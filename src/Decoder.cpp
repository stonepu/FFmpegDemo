#include "Decoder.h"


Decoder::Decoder()
{
	this->frame = nullptr;
	this->vmtx = new mutex;
}

//return 0 success, other failed.
int Decoder::sendPacket(AVPacket * pkt)
{
	if (!ctx) {
		cout << "a codecCtx is null" << endl;
		return 1;
	}
	vmtx->lock();
	int ret = avcodec_send_packet(ctx, pkt);
	vmtx->unlock();
	return ret;
}

AVFrame * Decoder::decode()
{
	if (!ctx) {
		return nullptr;
	}
	AVFrame* frame = av_frame_alloc();
	vmtx->lock();
	int ret = avcodec_receive_frame(ctx, frame);
	vmtx->unlock();
	if (ret < 0) {
		av_frame_free(&frame);
		return nullptr;
	}
	else return frame;
}

int Decoder::initCodec(AVCodecContext * codecCtx)
{
	this->ctx = codecCtx;
	this->codec = avcodec_find_decoder(codecCtx->codec_id);

	int ret = avcodec_open2(ctx, codec, 0);
	if (ret) {
		cout << "a codec context init error" << endl;
		return ret;
	}

	if (!frame) {
		frame = av_frame_alloc();
	}

	//avcodec_parameters_copy()
	return ret;
}
