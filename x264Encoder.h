/*===============================================================================

  Project: H264LiveStreamer
  Module: x264Encoder.h

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

#ifndef __X264_ENCODER_H
#define __X264_ENCODER_H

#ifdef __cplusplus
#define __STDINT_MACROS
#define __STDC_CONSTANT_MACROS
#endif

#include <iostream>
#include <concurrent_queue.h>
#include <opencv2/opencv.hpp>
#include <queue>
#include <stdint.h>

extern "C" {
#include <x264.h>
}

struct SwsContext;

class x264Encoder
{
 public:
  x264Encoder(void);
  ~x264Encoder(void);

 public:
  void initilize();
  void unInitilize();
  void encodeFrame(cv::Mat& image);
  bool isNalsAvailableInOutputQueue();
  x264_nal_t getNalUnit();
 private:
  // Use this context to convert your BGR Image to YUV image since x264 do not support RGB input
  SwsContext* convertContext;
  std::queue<x264_nal_t> outputQueue;
  x264_param_t parameters;
  x264_picture_t picture_in,picture_out;
  x264_t* encoder;
};

#endif
