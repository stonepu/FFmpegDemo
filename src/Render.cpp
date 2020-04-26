#include "Render.h"

AVRender::AVRender() {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS)) {
		throw Error("SDL init error");
	}

	window = SDL_CreateWindow("ffmpegDemo", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH,
		WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
	if (!window) {
		SDL_Quit();
		throw Error("SDL create window error");
	}

	render = SDL_CreateRenderer(window, -1, 0);
	texture = SDL_CreateTexture(render, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, WINDOW_WIDTH, WINDOW_HEIGHT);
}

AVRender::~AVRender() {
	SDL_CloseAudio();
	SDL_Quit();
	if (render) SDL_DestroyRenderer(render);
	if (texture) SDL_DestroyTexture(texture);
	if (window) SDL_DestroyWindow(window);
}

void AVRender::openAudio(int sampleRate, Uint8 channel, uint16_t samples, void* userdata,
	void(*fill_audio)(void *userData, Uint8 *stream, int len)) {
	audioSpec.freq = sampleRate;
	audioSpec.format = AUDIO_S16SYS;
	audioSpec.channels = channel;
	//audioSpec.silence = 0;
	audioSpec.samples = samples;
	audioSpec.callback = fill_audio;
	audioSpec.userdata = userdata;

	if (SDL_OpenAudio(&audioSpec, NULL) < 0) {
		throw Error("open audio error");
	}
	SDL_PauseAudio(0);
}


void AVRender::renderVideo(AVFrame *frame, uint32_t duration) {
	if (frame == nullptr) return;
	SDL_UpdateYUVTexture(texture, NULL, frame->data[0], frame->linesize[0],
		frame->data[1], frame->linesize[1],
		frame->data[2], frame->linesize[2]);
	SDL_RenderClear(render);
	SDL_RenderCopy(render, texture, NULL, NULL);
	SDL_RenderPresent(render);
	//SDL_Delay(duration);
}