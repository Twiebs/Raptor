#ifndef RAPTOR_PLATFORM_H
#define RAPTOR_PLATFORM_H

/*

#define PLATFORM_WINDOWS
#define PLATFORM_MAC
#define PLATFORM_LINUX
#define PLATFORM_ANDROID
#define PLATFORM_IOS
#define PLATFORM_HTML

#define APPLICATION_SDL
#define APPLICATION_GLFW

*/

static int InitImGui() {

}

#ifdef SDL_PLATFORM
#include <SDL2/SDL.h>
#undef main
#define PlatformGetCursorPos(xptr, yptr) SDL_GetMouseState(xptr, yptr)
#define PlatformGetCursorDelta(xptr, yptr) SDL_GetRelativeMouseState(xptr, yptr)
#define PlatformGetKey(keycode) __SDLPlatformGetKey(keycode)
//#define GetWindowSize(w_ptr, y_ptr) SDL_GetWindowSize(global_window, width, height);

static int __SDLPlatformGetKey(int keycode);

static int __SDLPlatformGetKey(int keycode) {
	static int keycount;
	static const Uint8* keystates = SDL_GetKeyboardState(&keycount);
	assert(keycode < keycount);
	return keystates[keycode];
}

#endif // SDL_PLATFORM


#endif	//RAPTOR_PLATFORM_H
