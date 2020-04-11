#pragma once

extern "C" {
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
}

#include "Decoder.h"
#include "util.h"

class VideoDecoder : public Decoder {
public:
	VideoDecoder();
	~VideoDecoder();

	void setWidth(int width);
	void setHeight(int height);
	AVFrame* receiveFrame() override;

private:
	SwsContext* swsCtx;

	int width = 640;
	int height = 360;
	AVPixelFormat pixFmt = AV_PIX_FMT_YUV420P;

	int initSws(AVFrame* frame);
};