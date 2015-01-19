/*===============================================================================

  Project: H264LiveStreamer
  Module: H264LiveServerMediaSession.h

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

#ifndef __H264LIVE_SERVER_MEDIA_SESSION_H
#define __H264LIVE_SERVER_MEDIA_SESSION_H

#include "liveMedia.hh"
#include "OnDemandServerMediaSubsession.hh"
#include "LiveSourceWithx264.h"

class H264LiveServerMediaSession:public OnDemandServerMediaSubsession
{
 public:
  static H264LiveServerMediaSession* createNew(UsageEnvironment& env, bool reuseFirstSource);
  void checkForAuxSDPLine1();
  void afterPlayingDummy1();
 protected:
  H264LiveServerMediaSession(UsageEnvironment& env, bool reuseFirstSource);
  virtual ~H264LiveServerMediaSession(void);
  void setDoneFlag() { fDoneFlag = ~0; }
 protected:
  virtual char const* getAuxSDPLine(RTPSink* rtpSink, FramedSource* inputSource);
  virtual FramedSource* createNewStreamSource(unsigned clientSessionId, unsigned& estBitrate);
  virtual RTPSink* createNewRTPSink(Groupsock* rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource* inputSource);
 private:
  char* fAuxSDPLine;
  char fDoneFlag;
  RTPSink* fDummySink;
};

#endif
