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

//Initializes the emscripten platform
int PlatformInit(const char* title, int width, int height, bool fullscreen) {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		LOG_ERROR("Unable to initialize SDL:" << SDL_GetError());
		return -1;
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_Surface* screen;
	screen = SDL_SetVideoMode(1280, 720, 0, SDL_OPENGL);
	if (!screen) {
		LOG_ERROR("Unable to set video mode: " << SDL_GetError());
		return -1;
	}

	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		LOG_ERROR("GLEW failed to initialize");
		return -1;
	}

	//Platform was initialized sucuessfuly
	return 0;
}

bool PlatformHandleInput() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_KEYDOWN:
			break;
		case SDL_KEYUP:
			break;
		case SDL_QUIT:
			return false;
			break;
		}
	}
	return true;
}

int PlatformShutdown() {
	SDL_Quit();
	//Platform shutdown suscuessfuly
	return 0;
}

void PlatformBeginFrame() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void PlatformEndFrame() {
	SDL_GL_SwapBuffers();
}

Pixmap* PlatformLoadPixmap(std::string& filename) {
	SDL_Surface* image = IMG_Load(filename.c_str());
	Pixmap* pixmap = new Pixmap();
	pixmap->width = image->w;
	pixmap->height = image->h;
	pixmap->data = (uint8*)image->pixels;
	return pixmap;
}

#endif  //_SDL
