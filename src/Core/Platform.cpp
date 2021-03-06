#include "Platform.h"

#include <Core/types.h>
#include <Core/logging.h>

global_variable bool global_running = true;
global_variable double global_deltaTime = 0.0;

extern "C" void PlatformExit() {
    global_running = false;
}

#if PLATFORM_SDL
#include <SDL2/SDL.h>
// TODO if we add non-gl renderering capabilies this should be removed!
#define GLEW_STATIC
#include <GL/glew.h>
#include <Graphics/GLRenderer.hpp>


global_variable SDL_Window* global_window;
global_variable SDL_GLContext global_context;
int PlatformCreate (const char* title, int width, int height, int flags) {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		LOG_ERROR("Unable to initialize SDL:" << SDL_GetError());
		return -1;
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	// SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

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

	// EnableGLDebugMode();
	return 0;
}

void PlatformGetSize(int* w, int* h) {
	return SDL_GetWindowSize(global_window, w, h);
}



U64 PlatformGetPerformanceCounter() {
	return SDL_GetPerformanceCounter();
}

U64 PlatformGetPerformanceCounterFrequency() { 
	return SDL_GetPerformanceFrequency();
}

double PlatformGetDeltaTime() {
	return global_deltaTime;
}

float PlatformGetNowFloat() {
	return ((float)(SDL_GetPerformanceCounter() * 1000) / (float)SDL_GetPerformanceFrequency());
}

double PlatformGetNow() {
	return ((double)(SDL_GetPerformanceCounter()*1000) / (double)SDL_GetPerformanceFrequency());
}

global_variable int keysPressedThisFrame[8];
global_variable int keysPressedThisFrameCount;

static void ResetInputEvents() { 
	keysPressedThisFrameCount = 0;
}

static void PushKeypress (int keycode) {
	keysPressedThisFrame[keysPressedThisFrameCount++] = keycode;
}

int PlatformPopKeypress() {
	return keysPressedThisFrame[--keysPressedThisFrameCount];
}

#include <Graphics/imgui.h>
static inline void ProcessSDLEvents() {
	SDL_Event event;
	SDL_GetRelativeMouseState(nullptr, nullptr);
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_KEYDOWN: {
			PushKeypress(event.key.keysym.scancode);
		} break;

		case SDL_QUIT:
		{
			global_running = false;
		} break;
		case SDL_TEXTINPUT: {
			auto& io = ImGui::GetIO();
			io.AddInputCharactersUTF8(event.text.text);
		} break;

		}
	}
}

void PlatformRun (const std::function<void(double)>& mainLoop) {
	while (global_running) {
		static U32 lastTime = SDL_GetTicks();
		U32 currentTime = SDL_GetTicks();
		global_deltaTime = ((double)(currentTime - lastTime)) / 1000.0f;
		lastTime = currentTime;

		ProcessSDLEvents();

		mainLoop(global_deltaTime);
		ResetInputEvents();
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
    global_deltaTime = (currentTime - lastTime) / 1000.0;
    lastTime = currentTime;

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

extern "C" double PlatformGetNow () {
	return emscripten_get_now();
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
