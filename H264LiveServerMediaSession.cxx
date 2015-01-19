/*===============================================================================

  Project: H264LiveStreamer
  Module: H264LiveServerMediaSession.cxx

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

#include "H264LiveServerMediaSession.h"

//------------------------------------------------------------------------------
H264LiveServerMediaSession*
H264LiveServerMediaSession::createNew(UsageEnvironment& env,
                                      bool reuseFirstSource)
{
  return new H264LiveServerMediaSession(env, reuseFirstSource);
}

H264LiveServerMediaSession::H264LiveServerMediaSession(UsageEnvironment& env,
                                                       bool reuseFirstSource)
  :OnDemandServerMediaSubsession(env,reuseFirstSource),
   fAuxSDPLine(NULL),
   fDoneFlag(0),
   fDummySink(NULL)
{
  //Do nothing
}

//------------------------------------------------------------------------------
H264LiveServerMediaSession::~H264LiveServerMediaSession(void)
{
  delete[] fAuxSDPLine;
}

//------------------------------------------------------------------------------
static void afterPlayingDummy(void* clientData)
{
  H264LiveServerMediaSession *session = (H264LiveServerMediaSession*)clientData;
  session->afterPlayingDummy1();
}

//------------------------------------------------------------------------------
void H264LiveServerMediaSession::afterPlayingDummy1()
{
  envir().taskScheduler().unscheduleDelayedTask(nextTask());
  setDoneFlag();
}

//------------------------------------------------------------------------------
static void checkForAuxSDPLine(void* clientData)
{
  H264LiveServerMediaSession* session = (H264LiveServerMediaSession*)clientData;
  session->checkForAuxSDPLine1();
}

//------------------------------------------------------------------------------
void H264LiveServerMediaSession::checkForAuxSDPLine1()
{
  char const* dasl;
  if(fAuxSDPLine != NULL)
    {
      setDoneFlag();
    }
  else if(fDummySink != NULL && (dasl = fDummySink->auxSDPLine()) != NULL)
    {
      fAuxSDPLine = strDup(dasl);
      fDummySink = NULL;
      setDoneFlag();
    }
  else
    {
      int uSecsDelay = 100000;
      nextTask() = envir().taskScheduler().scheduleDelayedTask(uSecsDelay, (TaskFunc*)checkForAuxSDPLine, this);
    }
}

//------------------------------------------------------------------------------
char const* H264LiveServerMediaSession::getAuxSDPLine(RTPSink* rtpSink, FramedSource* inputSource)
{
  if(fAuxSDPLine != NULL) return fAuxSDPLine;
  if(fDummySink == NULL)
    {
      fDummySink = rtpSink;
      fDummySink->startPlaying(*inputSource, afterPlayingDummy, this);
      checkForAuxSDPLine(this);
    }

  envir().taskScheduler().doEventLoop(&fDoneFlag);
  return fAuxSDPLine;
}

//------------------------------------------------------------------------------
FramedSource*
H264LiveServerMediaSession::createNewStreamSource(unsigned clientSessionID,
                                                  unsigned& estBitRate)
{
  /* comments from original source code*/
  // Based on encoder configuration i kept it 90000
  estBitRate = 90000;
  LiveSourceWithx264 *source = LiveSourceWithx264::createNew(envir());
  /* comments from original source code*/
  // are you trying to keep the reference of the source somewhere? you
  // shouldn't.
  // Live555 will create and delete this class object many times. if you store
  // it somewhere you will get memory access violation. instead you should
  // configure you source to always read from your data source.
  return H264VideoStreamDiscreteFramer::createNew(envir(),source);
}

//------------------------------------------------------------------------------
RTPSink*
H264LiveServerMediaSession::
createNewRTPSink(Groupsock* rtpGroupsock,
                 unsigned char rtpPayloadTypeIfDynamic,
                 FramedSource* inputSource)
{
  return H264VideoRTPSink::createNew(envir(), rtpGroupsock, rtpPayloadTypeIfDynamic);
}
