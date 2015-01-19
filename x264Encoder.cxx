/*===============================================================================

  Project: H264LiveStreamer
  Module: x264Encoder.cxx

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

#include "x264Encoder.h"

extern "C"{
#include <swscale.h>
}

x264Encoder::x264Encoder(void)
{

}


x264Encoder::~x264Encoder(void)
{

}

void x264Encoder::initilize()
{
  x264_param_default_preset(&parameters, "veryfast", "zerolatency");
  parameters.i_log_level = X264_LOG_INFO;
  parameters.i_threads = 1;
  parameters.i_width = 640;
  parameters.i_height = 480;
  parameters.i_fps_num = 25;
  parameters.i_fps_den = 1;
  parameters.i_keyint_max = 25;
  parameters.b_intra_refresh = 1;
  parameters.rc.i_rc_method = X264_RC_CRF;
  parameters.rc.i_vbv_buffer_size = 1000000;
  parameters.rc.i_vbv_max_bitrate = 90000;
  parameters.rc.f_rf_constant = 25;
  parameters.rc.f_rf_constant_max = 35;
  parameters.i_sps_id = 7;
  // the following two value you should keep 1
  parameters.b_repeat_headers = 1;    // to get header before every I-Frame
  parameters.b_annexb = 1; // put start code in front of nal. we will remove start code later
  x264_param_apply_profile(&parameters, "baseline");

  encoder = x264_encoder_open(&parameters);
  x264_picture_alloc(&picture_in, X264_CSP_I420, parameters.i_width, parameters.i_height);
  picture_in.i_type = X264_TYPE_AUTO;
  picture_in.img.i_csp = X264_CSP_I420;
  // i have initilized my color space converter for BGR24 to YUV420 because my opencv video capture gives BGR24 image. You can initilize according to your input pixelFormat
  convertContext = sws_getContext(parameters.i_width,parameters.i_height, PIX_FMT_BGR24, parameters.i_width,parameters.i_height,PIX_FMT_YUV420P, SWS_FAST_BILINEAR, NULL, NULL, NULL);
}

void x264Encoder::unInitilize()
{
  x264_encoder_close(encoder);
  sws_freeContext(convertContext);
}

void x264Encoder::encodeFrame(cv::Mat& image)
{
  int srcStride = parameters.i_width * 3;
  sws_scale(convertContext, &(image.data), &srcStride, 0, parameters.i_height, picture_in.img.plane, picture_in.img.i_stride);
  x264_nal_t* nals ;
  int i_nals = 0;
  int frameSize = -1;

  frameSize = x264_encoder_encode(encoder, &nals, &i_nals, &picture_in, &picture_out);
  if(frameSize > 0)
    {
    for(int i = 0; i< i_nals; i++)
      {
      outputQueue.push(nals[i]);
      }
    }
}

bool x264Encoder::isNalsAvailableInOutputQueue()
{
  if(outputQueue.empty() == true)
    {
    return false;
    }
  else
    {
    return true;
    }
}

x264_nal_t x264Encoder::getNalUnit()
{
  x264_nal_t nal;
  nal = outputQueue.front();
  outputQueue.pop();
  return nal;
}
