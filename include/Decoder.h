#pragma once

extern "C" {
#include "libavcodec/avcodec.h"
#include "libswresample/swresample.h"
}
#include <iostream>
#include <mutex>
using std::cout;
//using std::cin;
using std::endl;
using std::mutex;


class Decoder {
protected:
	AVCodec*			codec;
	AVCodecContext*		ctx;
	AVFrame*            frame; //�洢ת����ʽ������ݣ�
	mutex*				vmtx;
public:
	Decoder();
	virtual AVFrame* receiveFrame() { return nullptr; }
	virtual int initCodec(AVCodecContext* codecCtx);
	virtual int sendPacket(AVPacket* packet);
protected:

	virtual AVFrame* decode();

};