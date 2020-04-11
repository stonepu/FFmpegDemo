#pragma once

extern "C" {
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswresample/swresample.h"
}

#include <string>
#include <iostream>
#include "util.h"
#include <exception>

class Grabber {
public:
	Grabber();
	int open(const std::string& filename);
	AVCodecContext* getVideoCtx() { return vctx; }
	AVCodecContext* getAudioCtx() { return actx; }
	double getFrameRate() { return frameRate; }
	int getVideoIndex() { return videoIndex; }
	int getAudioIndex() { return audioIndex; }
	AVFrame* decode(AVPacket* pkt);
	int receiveFrame(AVFrame* frame);
	AVPacket* grab();

private:
	AVFormatContext*	fctx;
	AVCodecContext		*vctx, *actx;
	int					videoIndex, audioIndex;
	double				frameRate;


};