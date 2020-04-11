#include "Processor.h"

Processor::Processor()
{
	this->swrCtx = nullptr;
	this->swsCtx = nullptr;
}

Processor::Processor( long duration):duration(duration)
{
}

Processor::~Processor() {
	if (this->audioDecoder) {
		delete audioDecoder;
	}
	if (this->videoDecoder) {
		delete videoDecoder;
	}
	if (this->grabber) {
		delete grabber;
	}
	if (this->render) {
		delete render;
	}
}

void Processor::start()
{	
	this->duration = 1000 / grabber->getFrameRate();
	thread grabberThread(&Processor::startGrab, this);
	grabberThread.detach();

	thread drawThread(&Processor::startDraw, this);
	drawThread.detach();
}

void Processor::startGrab()
{
	bool needSlow = false;
	while (1) {
		if (vList.size() < 10) {
			needSlow = false;
			try {
				auto pkt = grabber->grab();
				//			auto frame = grabber->decode(pkt);
				if (pkt->stream_index == grabber->getAudioIndex()) {
					av_free_packet(pkt);
					//TODO 完成音频播放及音画同步
				}
				if (pkt->stream_index == grabber->getVideoIndex()) {
					auto frame = grabber->decode(pkt);
					av_packet_unref(pkt);
					vmtx.lock();
					if (frame && frame->format != -1) {
						vList.push_back(frame);
					}
					vmtx.unlock();
				}
			}
			catch (exception e) {
				cout << e.what() << endl;
				break;
			}
		}
		else {
			needSlow = true;
		}
		if (needSlow) {
			std::this_thread::sleep_for(std::chrono::milliseconds(this->duration / 5));
		}
	}
}

void Processor::startDraw()
{
	while (1)
	{
		AVFrame* frame = nullptr;
		vmtx.lock();
		if (vList.size() > 0) {
			frame = vList.front();
			vList.pop_front();
		}
		vmtx.unlock();
		if (frame) {
			auto nFrame = convertVFrame(frame);
			render->renderVideo(nFrame, duration);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(duration/2));
	}
	cout << "video player end." << endl;
}

int Processor::iniSwsCtx(AVFrame* preFrame)
{
	if (!newFrame) {
		newFrame = av_frame_alloc();
	}
	cout << "pre->width: " << preFrame->width << endl;
	cout << "pre->height" << preFrame->height << endl;
	cout << "pre->fmt" << preFrame->format << endl;
	cout << "f->wid" << this->width << endl;
	cout << "f->height" << this->height << endl;
	cout << "f->fmt" << this->pixFmt << endl;
	this->swsCtx = sws_getCachedContext(swsCtx, preFrame->width, preFrame->height, (AVPixelFormat)preFrame->format, this->width, this->height, this->pixFmt, SWS_BICUBIC, NULL, NULL, NULL);

	uint8_t *out_buffer = (uint8_t *)av_malloc(av_image_get_buffer_size(this->pixFmt, this->width, this->height, 0) * sizeof(uint8_t));
	av_image_fill_arrays(newFrame->data, newFrame->linesize, out_buffer, this->pixFmt, this->width, this->height, 0);
	return 0;
}

AVFrame * Processor::convertVFrame(AVFrame * preFrame)
{
	if (!swsCtx) iniSwsCtx(preFrame);
	newFrame->data[0] = preFrame->data[0];
	newFrame->data[1] = preFrame->data[1];
	newFrame->data[2] = preFrame->data[2];
	newFrame->data[3] = preFrame->data[3];
	newFrame->linesize[0] = preFrame->linesize[0];
	newFrame->linesize[1] = preFrame->linesize[1];
	newFrame->linesize[2] = preFrame->linesize[2];
	newFrame->linesize[3] = preFrame->linesize[3];
	sws_scale(swsCtx, preFrame->data, preFrame->linesize, 0, preFrame->height, newFrame->data, newFrame->linesize);
	av_frame_free(&preFrame);
	return newFrame;
}
