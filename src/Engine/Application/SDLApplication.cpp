#ifdef SDL_APP
#include "Application.hpp"
#include <SDL/SDL.h>

void Application::Init(const char* tille, uint32 width, uint32 height) {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		LOG_ERROR("Unable to initialize SDL:" << SDL_GetError());
		return -1;
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_Surface* screen;

#ifndef _MSC_VER
	screen = SDL_SetVideoMode(1280, 720, 0, SDL_OPENGL);
	if (!screen) {
		LOG_ERROR("Unable to set video mode: " << SDL_GetError());
		return -1;
	}
#else
	//TODO fullscren does nothing...
	window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
	if (!window)
		LOG_ERROR(SDL_GetError());
	context = SDL_GL_CreateContext(window);
#endif
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		LOG_ERROR("GLEW failed to initialize");
		return -1;
	}

	//Platform was initialized sucuessfuly
	return 0;
}

void Application::Destroy() {
	SDL_Quit();
}

void Application::BeginFrame() {
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Application::EndFrame() {
	#ifdef _MSC_VER
		SDL_GL_SwapWindow(window);
	#else
		SDL_GL_SwapBuffers();
	#endif
}

void Application::PollEvents() {
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_KEYDOWN:
			break;
		case SDL_KEYUP:
			break;
		case SDL_QUIT:
			break;
		}
	}

}

#endif SDL_APP
