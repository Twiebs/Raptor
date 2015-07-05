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
