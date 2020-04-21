#pragma once
#include <SDL.h>
#include <iostream>
#include "util.h"
extern "C" {
#include "libavutil/frame.h"
}

constexpr int WINDOW_WIDTH = 640;
constexpr int WINDOW_HEIGHT = 360;

#define REFRESH_EVENT (SDL_USEREVENT + 1)
#define END_EVENT (SDL_USEREVENT + 2)


class AVRender {
public:
	AVRender();
	~AVRender();
	void openAudio(int sampleRate, Uint8 channel, uint16_t samples, void* userdata,
		void(*fill_audio)(void *codecContext, Uint8 *stream, int len));

	void renderVideo(AVFrame *frame, uint32_t duration);

private:
	SDL_Window *window;
	SDL_Renderer *render;
	SDL_Texture *texture;
	SDL_AudioSpec audioSpec;
};