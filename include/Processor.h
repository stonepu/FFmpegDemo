#pragma once

#include "AudioDecoder.h"
#include "VideoDecoder.h"
#include "Render.h"
#include "Grabber.h"
#include <thread>
#include <chrono>
#include <list>
#include <vector>

extern "C" {
#include "libswresample/swresample.h"
#include "libswscale/swscale.h"
}

using std::thread;
using std::list;
using std::vector;

class Processor {
public:
	Processor();
	Processor(long duration);
	~Processor();
	void start();
	void setGrabber(Grabber* grabber) { this->grabber = grabber; }
	void setADecoder(AudioDecoder* aDecoder) { this->audioDecoder = aDecoder; }
	void setVDecoder(VideoDecoder* vDecoder) { this->videoDecoder = vDecoder; }
	void setRender(AVRender* render) { this->render = render; }

private:
	Grabber*		grabber;
	AVRender*		render;
	AudioDecoder*	audioDecoder;
	VideoDecoder*	videoDecoder;
	SwsContext*		swsCtx;
	SwrContext*		swrCtx;
	long			duration = 33;
	list<AVFrame*> vList{}, aList{}, pktList{};
	std::mutex		vmtx;


	//video
	int				width = 640,
					height = 360;
	AVPixelFormat	pixFmt = AV_PIX_FMT_YUV420P;
	AVFrame*		newFrame = nullptr;


	void startGrab();
	void startDraw();
	int iniSwrCtx(AVFrame* frame);
	int iniSwsCtx(AVFrame* frame);
	AVFrame* convertVFrame(AVFrame* preFrame);
};