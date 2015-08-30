#include "Platform.h"

#include <Core/Common.hpp>
#if PLATFORM_SDL
#include <SDL2/SDL.h>
#define GLEW_STATIC
#include <GL/glew.h>
global_variable SDL_Window* global_window;
global_variable SDL_GLContext global_context;
global_variable bool global_running = true;
global_variable double global_deltaTime = 0.0;
int PlatformCreate(const char* title, int width, int height, int flags) {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
		LOG_ERROR("Unable to initialize SDL:" << SDL_GetError());
		return -1;
	}
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);

	SDL_Surface* screen;
	//TODO fullscren does nothing...
	global_window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
	if (!global_window)
		LOG_ERROR(SDL_GetError());
	global_context = SDL_GL_CreateContext(global_window);

	if (flags) SDL_SetWindowFullscreen(global_window, SDL_WINDOW_FULLSCREEN_DESKTOP);

	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		LOG_ERROR("GLEW failed to initialize");
		return -1;
	}
	return 0;
}

void PlatformGetSize(int* w, int* h) {
	return SDL_GetWindowSize(global_window, w, h);
}

double PlatformGetDeltaTime() {
	return global_deltaTime;
}

void PlatformRun(void(*mainLoop)(double)) {
	while (global_running) {
		static double lastTime = SDL_GetTicks();
		double currentTime = SDL_GetTicks();
		global_deltaTime = (currentTime - lastTime) / 1000.0f;

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT: {
				global_running = false;
			} break;
			case SDL_TEXTINPUT: {
			} break;

			}
		}

		mainLoop(global_deltaTime);
		SDL_GL_SwapWindow(global_window);
	}
}

void PlatformExit() {
	global_running = false;
}

#endif