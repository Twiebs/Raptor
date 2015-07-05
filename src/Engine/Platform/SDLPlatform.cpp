//This file is only used if were compiling with emscrpiten
#ifdef _SDL
#include <iostream>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

#include <Core/Common.hpp>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <GL/glew.h>
#include "Platform.hpp"

#include <Graphics/Pixmap.hpp>

#ifdef _MSC_VER
SDL_Window* window;
SDL_GLContext context;
#endif

bool keysDown[1024];

int PlatformInit(const char* title, int width, int height, bool fullscreen) {
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

int PlatformShutdown() {
	SDL_Quit();
	return 0;
}

void PlatformBeginFrame() {
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void PlatformEndFrame() {
	#ifdef _MSC_VER
		SDL_GL_SwapWindow(window);
	#else
		SDL_GL_SwapBuffers();
	#endif
}

bool PlatformHandleInput() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_KEYDOWN:
			keysDown[event.key.keysym.sym] = true;
			break;
		case SDL_KEYUP:
			keysDown[event.key.keysym.sym] = false;
			break;
		case SDL_QUIT:
			break;
		}
	}
}
//#endif SDL_APP
Pixmap* PlatformLoadPixmap(std::string& filename) {
	SDL_Surface* image = IMG_Load(filename.c_str());
	if (image == nullptr) {
		LOG_ERROR("Could not open file: " << filename  << " :: "<< IMG_GetError());
		return nullptr;
	}

	Pixmap* pixmap = new Pixmap();
	pixmap->width = image->w;
	pixmap->height = image->h;
	pixmap->data = (uint8*)image->pixels;
	return pixmap;
}

#endif  //_SDL
