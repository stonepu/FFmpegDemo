#include "Processor.h"

void fillAudio(void* userData, uint8_t* data, int len) {
	Processor* processor = (Processor*)userData;
	processor->writeData(data, len);
}

Processor::Processor()
{
	this->swrCtx = nullptr;
	this->swsCtx = nullptr;
}

Processor::Processor( long duration):duration(duration)
{
	this->swrCtx = nullptr;
	this->swsCtx = nullptr;
}

Processor::~Processor() {

	if (this->grabber) {
		delete grabber;
	}
	if (this->render) {
		delete render;
	}
}

void Processor::writeData(uint8_t * data, int len)
{
	SDL_memset(data, 0, len);

	std::lock_guard<std::mutex> locker(amtx);
	if (aList.empty()) return;
	auto frame = aList.back();
	aList.pop_back();

	auto newFrame = convertAFrame(frame);
	auto buffSize = av_samples_get_buffer_size(newFrame->linesize, newFrame->channels, newFrame->nb_samples, (AVSampleFormat)newFrame->format, 0);
	if (buffSize != len) {
		av_log(NULL, AV_LOG_INFO, "buffSize != len \n");
	}

	SDL_MixAudio(data, newFrame->data[0], buffSize, SDL_MIX_MAXVOLUME);
	av_frame_free(&newFrame);
}

void Processor::start()
{	
	this->duration = (long) 1000 / grabber->getFrameRate();
	std::cout << duration << std::endl;

	auto frameSize = grabber->getAudioCtx()->frame_size;

	std::cout << "frameSize: " << frameSize << std::endl;
	std::cout << "sampleRate: " << sampleRate << std::endl;
	std::cout << "channels: " << channels << std::endl;

	render->openAudio(sampleRate, channels, frameSize, (void* )this, fillAudio);

	thread grabberThread(&Processor::startGrab, this);
	grabberThread.detach();
}

void Processor::startGrab()
{
	bool needSlow = false;
	while (1) {
		if ( aList.size() < 10) {
			needSlow = false;
			try {
				auto pkt = grabber->grab();
				if (pkt->stream_index == grabber->getAudioIndex()) {
					//av_free_packet(pkt);
					//TODO 完成音频播放及音画同步
					auto frame = grabber->decode(pkt);
					//av_packet_unref(pkt);
					if (frame && frame->format != -1) {
					//	//av_log(NULL, AV_LOG_INFO, "grab a frame\n");
						std::lock_guard<std::mutex> locker(amtx);
					//	amtx.lock();
						aList.push_back(frame);						
						//amtx.unlock();
					}
				}
				if (pkt->stream_index == grabber->getVideoIndex()) {
					auto frame = grabber->decode(pkt);
					av_packet_unref(pkt);
					vmtx.lock();
					if (frame && frame->format != -1) {
						vList.push_back(std::move(frame));
						SDL_Event event;
						event.type = REFRESH_EVENT;
						SDL_PushEvent(&event);
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
		SDL_Event event;
		SDL_PollEvent(&event);
		auto currDuration = duration;
		if (event.type == REFRESH_EVENT) {
			AVFrame* frame = nullptr;
			{
				std::lock_guard<std::mutex> locker(vmtx);
				if (vList.size() > 0) {
					frame = vList.front();
					vList.pop_front();
				}
				//std::cout << "listSize: " << vList.size() << std::endl;
				//控制音画同步;
				if (vList.size() > 20) currDuration = duration*0.5;
				else if (vList.size() > 10)currDuration = duration * 0.6;
				else if (vList.size() >= 5) currDuration = duration * 0.7;
				else if (vList.size() > 2) currDuration = duration * 0.9;
			}
			if (frame) {
				auto nFrame = convertVFrame(frame);
				render->renderVideo(nFrame, duration);
				std::this_thread::sleep_for(std::chrono::milliseconds(currDuration));
			}			
		}
	}
	cout << "video player end." << endl;
}

int Processor::iniSwsCtx(AVFrame* preFrame)
{
	if (!newFrame) {
		newFrame = av_frame_alloc();
	}
	newFrame->width = this->width;
	newFrame -> height = this->height;
	newFrame->format = this->pixFmt;


	cout << "pre->width: " << preFrame->width << endl;
	cout << "pre->height" << preFrame->height << endl;
	cout << "pre->fmt" << preFrame->format << endl;
	cout << "f->wid" << this->width << endl;
	cout << "f->height" << this->height << endl;
	cout << "f->fmt" << this->pixFmt << endl;
	this->swsCtx = sws_getCachedContext(swsCtx, preFrame->width, preFrame->height, (AVPixelFormat)preFrame->format, this->width, this->height, this->pixFmt, SWS_FAST_BILINEAR, NULL, NULL, NULL);

	uint8_t *out_buffer = (uint8_t *)av_malloc(av_image_get_buffer_size(this->pixFmt, this->width, this->height, 0) * sizeof(uint8_t));
	av_image_fill_arrays(newFrame->data, newFrame->linesize, out_buffer, this->pixFmt, this->width, this->height, 0);
	return 0;
}

int Processor::iniSwrCtx(AVFrame * frame)
{
	sampleRate = frame->sample_rate;
	swrCtx = swr_alloc_set_opts(swrCtx, av_get_default_channel_layout(channels), sampleFmt, sampleRate, av_get_default_channel_layout(channels), (AVSampleFormat)frame->format, sampleRate, 0, NULL);

	swr_init(swrCtx);
	if (!swrCtx) {
		av_log(NULL, AV_LOG_ERROR, "swrContext init failed!");
		return -1;
	}

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

AVFrame * Processor::convertAFrame(AVFrame * preFrame)
{
	int ret = 0;
	if (!swrCtx) iniSwrCtx(preFrame);
	AVFrame* newFrame = av_frame_alloc();
	newFrame->channels = 2;
	newFrame->nb_samples = preFrame->nb_samples;
	newFrame->format = sampleFmt;
	newFrame->channel_layout = av_get_default_channel_layout(channels);

	ret = av_frame_get_buffer(newFrame, 0);
	if (ret < 0) {
		av_log(NULL, AV_LOG_ERROR, "a frame get buffer error \n");
		return nullptr;
	}

	auto buffSize = av_samples_get_buffer_size(newFrame->linesize, newFrame->channels, newFrame->nb_samples, sampleFmt, 0);

	swr_convert(swrCtx, (uint8_t **)newFrame->data, buffSize, (const uint8_t **)preFrame->data, preFrame->nb_samples);

	av_frame_free(&preFrame);

	return newFrame;
}
