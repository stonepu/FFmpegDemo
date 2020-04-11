#include "VideoDecoder.h"

VideoDecoder::VideoDecoder()
{
	this->swsCtx = nullptr;
	this->width = 640;
	this->height = 360;
}

VideoDecoder::~VideoDecoder()
{
	if (swsCtx) sws_freeContext(swsCtx);
}

//¿ÉÄÜ·µ»Ønullptr;
AVFrame * VideoDecoder::receiveFrame()
{	
	auto preFrame = decode();
	if (!preFrame) return nullptr;
	if (!swsCtx) initSws(preFrame);
	//sws_getCachedContext(swsCtx, frame->width, frame->height, (AVPixelFormat)frame->format, ctx->width, ctx->height, ctx->pix_fmt, SWS_BICUBIC, NULL, NULL, NULL);
	//AVFrame* newF = av_frame_alloc();
	frame->data[0] = preFrame->data[0];
	frame->data[1] = preFrame->data[1];
	frame->data[2] = preFrame->data[2];
	frame->data[3] = preFrame->data[3];
	frame->linesize[0] = preFrame->linesize[0];
	frame->linesize[1] = preFrame->linesize[1];
	frame->linesize[2] = preFrame->linesize[2];
	frame->linesize[3] = preFrame->linesize[3];

	sws_scale(swsCtx, preFrame->data, preFrame->linesize, 0, preFrame->height, frame->data, frame->linesize);	
	av_frame_free(&preFrame);
	return frame;
}

int VideoDecoder::initSws(AVFrame * preFrame)
{
	cout << "pre->width: " << preFrame->width << endl;
	cout << "pre->height" << preFrame->height << endl;
	cout << "pre->fmt" << preFrame->format << endl;
	cout << "f->wid" << this->width << endl;
	cout << "f->height" << this->height << endl;
	cout << "f->fmt" << this->pixFmt << endl;
	this->swsCtx = sws_getCachedContext(swsCtx, frame->width, frame->height, (AVPixelFormat)frame->format, this->width, this->height, this->pixFmt, SWS_BICUBIC, NULL, NULL, NULL);

	uint8_t *out_buffer = (uint8_t *)av_malloc(av_image_get_buffer_size(this->pixFmt, this->width, this->height, 0) * sizeof(uint8_t));
	av_image_fill_arrays(frame->data, frame->linesize, out_buffer, this->pixFmt, this->width, this->height, 0);
	return 0;
}
