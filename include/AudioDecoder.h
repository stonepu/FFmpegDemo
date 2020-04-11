#pragma once

#include "Decoder.h"
extern "C" {
#include "libswresample/swresample.h"
}
#include <iostream>

using std::cout;
//using std::cin;
using std::endl;

class AudioDecoder : public Decoder {
public:
	AudioDecoder();
	~AudioDecoder();
	
	int initSwr(AVFrame* frame);
	//int initCodec(AVCodecContext* codecCtx) override;
	//int sendPacket(AVPacket* packet) override;
	//AVFrame* decode() override;
	AVFrame* receiveFrame() override;


private:
	SwrContext* swrCtx;

	int channels = 2;
	int sampleRate =  44100;
	AVSampleFormat intFmt, outFmt;

};