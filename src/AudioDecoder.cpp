#include "AudioDecoder.h"

AudioDecoder::AudioDecoder():Decoder()
{
	this->codec = nullptr;
	this->ctx = nullptr;
	this->swrCtx = nullptr;
}

AudioDecoder::~AudioDecoder()
{
	if (swrCtx) swr_free(&swrCtx);
}

int AudioDecoder::initSwr(AVFrame * frame)
{

	swrCtx = swr_alloc_set_opts(swrCtx, av_get_default_channel_layout(channels), AV_SAMPLE_FMT_FLTP, sampleRate, av_get_default_channel_layout(channels), (AVSampleFormat)frame->format, sampleRate, 0, NULL);

	swr_init(swrCtx);
	if (!swrCtx) {
		std::cout << "swrContext init failed!" << std::endl;
		return -1;
	}

	//if (!(m_converted_input_samples = (uint8_t**)calloc(actx->channels, sizeof(**m_converted_input_samples))))
	//{
	//	std::cout << "Could not allocate converted input sample pointers\n" << std::endl;;
	//	return -1;
	//}

	return 0;
}

AVFrame * AudioDecoder::receiveFrame()
{
	return nullptr;
}




