/*===============================================================================

  Project: H264LiveStreamer
  Module: LiveSourceWithx264.cxx

  Copyright (c) 2014-2015, Rafael Palomar <rafaelpalomaravalos@gmail.com>

  Permission to use, copy, modify, and/or distribute this software for any
  purpose with or without fee is hereby granted, provided that the above
  copyright notice and this permission notice appear in all copies.

  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

  =============================================================================*/

#include "LiveSourceWithx264.h"

LiveSourceWithx264* LiveSourceWithx264::createNew(UsageEnvironment& env)
{
  return new LiveSourceWithx264(env);
}

EventTriggerId LiveSourceWithx264::eventTriggerId = 0;

unsigned LiveSourceWithx264::referenceCount = 0;

LiveSourceWithx264::LiveSourceWithx264(UsageEnvironment& env):FramedSource(env)
{
  if(referenceCount == 0)
    {

    }
  ++referenceCount;
  videoCaptureDevice.open(0);
  encoder = new x264Encoder();
  encoder->initilize();
  if(eventTriggerId == 0)
    {
      eventTriggerId = envir().taskScheduler().createEventTrigger(deliverFrame0);
    }
}


LiveSourceWithx264::~LiveSourceWithx264(void)
{
  --referenceCount;
  videoCaptureDevice.release();
  encoder->unInitilize();
  envir().taskScheduler().deleteEventTrigger(eventTriggerId);
  eventTriggerId = 0;
}

void LiveSourceWithx264::encodeNewFrame()
{
  rawImage.data = NULL;
  while(rawImage.data == NULL)
    {
      videoCaptureDevice >> rawImage;
      cv::waitKey(100);
    }
  // Got new image to stream
  assert(rawImage.data != NULL);
  encoder->encodeFrame(rawImage);
  // Take all nals from encoder output queue to our input queue
  while(encoder->isNalsAvailableInOutputQueue() == true)
    {
      x264_nal_t nal = encoder->getNalUnit();
      nalQueue.push(nal);
    }
}

void LiveSourceWithx264::deliverFrame0(void* clientData)
{
  ((LiveSourceWithx264*)clientData)->deliverFrame();
}

void LiveSourceWithx264::doGetNextFrame()
{
  if(nalQueue.empty() == true)
    {
      encodeNewFrame();
      gettimeofday(&currentTime,NULL);
      deliverFrame();
    }
  else
    {
      deliverFrame();
    }
}

void LiveSourceWithx264::deliverFrame()
{
  if(!isCurrentlyAwaitingData()) return;
  x264_nal_t nal = nalQueue.front();
  nalQueue.pop();
  assert(nal.p_payload != NULL);
  // You need to remove the start code which is there in front of every nal unit.
  // the start code might be 0x00000001 or 0x000001. so detect it and remove it. pass remaining data to live555
  int trancate = 0;
  if (nal.i_payload >= 4 && nal.p_payload[0] == 0 && nal.p_payload[1] == 0 && nal.p_payload[2] == 0 && nal.p_payload[3] == 1 )
    {
      trancate = 4;
    }
  else
    {
      if(nal.i_payload >= 3 && nal.p_payload[0] == 0 && nal.p_payload[1] == 0 && nal.p_payload[2] == 1 )
        {
          trancate = 3;
        }
    }

  if(nal.i_payload-trancate > fMaxSize)
    {
      fFrameSize = fMaxSize;
      fNumTruncatedBytes = nal.i_payload-trancate - fMaxSize;
    }
  else
    {
      fFrameSize = nal.i_payload-trancate;
    }
  fPresentationTime = currentTime;
  memmove(fTo,nal.p_payload+trancate,fFrameSize);
  FramedSource::afterGetting(this);
}  
