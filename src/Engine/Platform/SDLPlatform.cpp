//This file is only used if were compiling with emscrpiten
#ifdef _SDL
#include <iostream>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif


#include <GL/glew.h>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

#include "Platform.hpp"
//Initalizes the emscripten platform
int PlatformInit(const char* title, int width, int height, bool fullscreen) {
  if(SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cout << "Unable to initalize SDL:" << SDL_GetError();
    return -1;
  }

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  //SDL_Surface* screen;
  //screen = SDL_SetVideoMode(1280, 720, 0, SDL_OPENGL);
  // if(!screen) {
  //   std::cout << "Unabled to set video mode: " << SDL_GetError();
  //   return -1;
  // }
  //
  // if(glewInit() != GLEW_OK) {
  //   std::cout << "GLEW failed to initalize";
  //   return -1;
  // }

  //Platform was initalized suscuessfuly
  //return 0;
}

int PlatformShutdown() {
  SDL_Quit();
  //Platform shutdown suscuessfuly
  return 0;
}

void PlatformBeginFrame() {
  glClearColor(1.0f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
}

void PlatformEndFrame() {
  // SDL_GL_SwapBuffers();
}

#endif  //_SDL
