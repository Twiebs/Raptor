#pragma once

#include <SDL\SDL.h>
#undef main	//SDL is silly
#include <Core\Common.hpp>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

static bool PLATFORMInit(const char* title, uint32 width, uint height, bool fullscreen) {
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	SDL_Window* window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, fullscreen ? SDL_WINDOW_FULLSCREEN : 0);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	return true;
}

static void PLATFORMShutdown() {
	SDL_Quit();
}

static void PLATFORMHandleInputEvents() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			PLATFORMShutdown();
			break;
		}
	}
}

