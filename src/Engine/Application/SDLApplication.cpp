#ifdef SDL

#include <Application/Application.hpp>

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_net.h>
#undef main

SDL_Window* window;
SDL_GLContext context;

#define AUDIO_FREQUENCY MIX_DEFAULT_FREQUENCY * 2
#define AUDIO_FORMAT MIX_DEFAULT_FORMAT
#define AUDIO_CHANNELS MIX_DEFAULT_CHANNELS
#define AUDIO_CHUNK_SIZE 1024

int Application::Create(const char* title, uint32 width, uint32 height, bool fullscreen) {
	this->width = width;
	this->height = height;
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
		LOG_ERROR("Unable to initialize SDL:" << SDL_GetError());
		return -1;
	}

	int mixFlags = MIX_INIT_OGG | MIX_INIT_FLAC | MIX_INIT_MP3;
	int initalized = Mix_Init(mixFlags);
	if (initalized & mixFlags != mixFlags) {
		LOG_ERROR("AUDIO: Failed to init required audio library support");
		LOG_ERROR("AUDIO: " << Mix_GetError());
	}

	if (Mix_OpenAudio(AUDIO_FREQUENCY, AUDIO_FORMAT, AUDIO_CHANNELS, AUDIO_CHUNK_SIZE)) {
		LOG_ERROR("AUDIO: Failed to initalize audio context" << Mix_GetError());
	}


	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_Surface* screen;
	//TODO fullscren does nothing...
	window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
	if (!window)
		LOG_ERROR(SDL_GetError());
	context = SDL_GL_CreateContext(window);

	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		LOG_ERROR("GLEW failed to initialize");
		return -1;
	}

	//Platform was initialized sucuessfuly
	isRunning = true;
	return 0;
}

int Application::Destroy() {
	Mix_CloseAudio();
	Mix_Quit();
	SDL_Quit();
	return 0;
}

void Application::BeginFrame() {
	static float64 lastTime = GetTime();
	float64 currentTime = GetTime();
	deltaTime = (currentTime - lastTime) / 1000.0f;
	lastTime = GetTime();

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Application::EndFrame() {
	mouseWheel = 0;
	SDL_GL_SwapWindow(window);
}

float64 Application::GetTime() {
	return (float64)SDL_GetTicks();
}

void Application::PollEvents() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_KEYDOWN:
			if (keyCallback != nullptr) keyCallback(event.key.keysym.scancode, true);
			keysDown[event.key.keysym.scancode] = true;
			break;
		case SDL_KEYUP:
			if (keyCallback != nullptr) keyCallback(event.key.keysym.scancode, false);
			keysDown[event.key.keysym.scancode] = false;
			break;
		case SDL_MOUSEMOTION:
			cursorX = (float64)event.motion.x;
			cursorY = (float64)event.motion.y;
			break;
		case SDL_MOUSEWHEEL:
			mouseWheel = event.wheel.y;
			break;
		case SDL_MOUSEBUTTONDOWN:
			buttonsDown[event.button.button] = true;
			break;
		case SDL_MOUSEBUTTONUP:
			buttonsDown[event.button.button] = false;
			break;
		case SDL_QUIT:
			isRunning = false;
			break;
		}
	}
}

#endif //SDL
