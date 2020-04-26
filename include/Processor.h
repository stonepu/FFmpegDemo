#pragma once

#include "Render.h"
#include "Grabber.h"
#include <thread>
#include <chrono>
#include <list>
#include <vector>
#include <mutex>

extern "C" {
#include "libswresample/swresample.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
}

using std::thread;
using std::list;
using std::vector;


class Processor {
public:
	Processor();
	Processor(long duration);
	~Processor();
	void writeData(uint8_t * data, int len);
	void start();
	void setGrabber(Grabber* grabber) { this->grabber = grabber; }
	void setRender(AVRender* render) { this->render = render; }
	void startDraw();


private:
	Grabber*		grabber;
	AVRender*		render;
	SwsContext*		swsCtx;
	SwrContext*		swrCtx;
	long			duration = 33;
	//list<AVFrame*>  
	list<AVPacket*> vList{}, aList{};;
	std::mutex		vmtx;
	std::mutex		amtx;


	//video
	int				width = 640,
					height = 360;
	AVPixelFormat	pixFmt = AV_PIX_FMT_YUV420P;
	AVFrame*		newFrame = nullptr;


	//audio
	int				channels = 2;
	int				sampleRate = 44100;
	AVSampleFormat  sampleFmt = AV_SAMPLE_FMT_S16;

	void startGrab();
	int iniSwrCtx(AVFrame* frame);
	int iniSwsCtx(AVFrame* frame);
	AVFrame* convertVFrame(AVFrame* preFrame);
	AVFrame * convertAFrame(AVFrame * preFrame);
};