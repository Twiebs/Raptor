#include "Platform.h"

#include <Core/Common.hpp>
global_variable bool global_running = true;
global_variable double global_deltaTime = 0.0;

extern "C" void PlatformExit() {
    global_running = false;
}

#if PLATFORM_SDL
#include <SDL2/SDL.h>
#define GLEW_STATIC
#include <GL/glew.h>

global_variable SDL_Window* global_window;
global_variable SDL_GLContext global_context;

int PlatformCreate(const char* title, int width, int height, int flags) {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		LOG_ERROR("Unable to initialize SDL:" << SDL_GetError());
		return -1;
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);

	SDL_Surface* screen;
	//TODO fullscreen does nothing...
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

#endif

#ifdef PLATFORM_HTML5
#include <GL/glew.h>
#include <SDL/SDL.h>
#undef main
#include <emscripten/emscripten.h>

global_variable SDL_Surface* global_surface;
extern "C" int PlatformCreate (const char* title, int width, int height, int flags) {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		LOG_ERROR("Unable to initialize SDL:" << SDL_GetError());
		return -1;
	}
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    global_surface = SDL_SetVideoMode(1280, 720, 0, SDL_OPENGL);
    if (!global_surface) {
        LOG_ERROR("Unable to set video mode: " << SDL_GetError());
        return -1;
    }

    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        LOG_ERROR("GLEW failed to initialize");
        return -1;
    }
    return 0;
}

inline static void __EmscriptenMainLoop(void* mainLoopPtr) {
    auto mainLoop = (void(*)(double))mainLoopPtr;
    static double lastTime = emscripten_get_now();
    double currentTime = emscripten_get_now();
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
    SDL_GL_SwapBuffers();
}


extern "C" void PlatformRun(void(*mainLoop)(double)) {
    void* arg = (void*)mainLoop;
    emscripten_set_main_loop_arg(__EmscriptenMainLoop, arg, 0, true);
}

extern "C" double PlatformGetDeltaTime() {
    return emscripten_get_now();
}

extern "C" void PlatformGetSize(int* w, int* h) {
	*w = global_surface->w;
	*h = global_surface->h;
}

#endif
