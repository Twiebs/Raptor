#ifndef RAPTOR_PLATFORM_H
#define RAPTOR_PLATFORM_H

#ifdef SDL_PLATFORM
#include <SDL2/SDL.h>
#define PlatformGetCursorPos(xptr, yptr) SDL_GetMouseState(xptr, yptr)
#define PlatformGetCursorDelta(xptr, yptr) SDL_GetRelativeMouseState(xptr, yptr)
#define PlatformGetKey(keycode) __SDLPlatformGetKey(keycode)

int __SDLPlatformGetKey(int keycode);

int __SDLPlatformGetKey(int keycode) {
	static int keycount;
	static const Uint8* keystates = SDL_GetKeyboardState(&keycount);
	assert(keycode < keycount);
	return keystates[keycode];
}

#endif // SDL

#endif	//RAPTOR_PLATFORM_H
