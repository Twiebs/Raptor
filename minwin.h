/*
	===========================================================================================
	Minwin - Ultra minimial, cross plaform, single file, window creation library;
	Version: 0.1
	Author: Torin Wiebelt 2015
	Licence: This file is in the public domain.  No Warranty is implied.  Use at your own risk.
		The author is intrested in hearing how is work is being used however.
		If you like you can send him an email torinwiebelt@gmail.com
		(Note Author is unsure why he is speaking in the 3rd person)
  ===========================================================================================
	This project lives at http://github.com/Twiebs/Minwin

	Usage:
		- #define MINWIN_IMPLEMENTATION in one C or C++ file to create the implementation of the libarary
				#include ...
				#include ...
				#define MINWIN_IMPLEMENTATION
				#include "minwin.h"
				By default this will generate an implementation for the current platform you are compiling on.
				if you want to compile with a different implementation use:
					-  MINWIN_LINUX_IMPLEMENTATION
					-  MINWIN_WINDOWS_IMPLEMENTATION
					-  MINWIN_APPLE_IMPLEMENTATION
					-  MINWIN_ANDROID_IMPLEMENTATION

	Configuration
		- #define MINWIN_ERROR(msg) to use custom error loging otherwise stdio.h is inlcuded
*/
#ifndef MINWIN_INCLUDE_GUARD
#define MINWIN_INCLUDE_GUARD

#ifndef MINWIN_ERROR
#include <stdio.h>
#define MINWIN_ERROR(msg) fprintf(stderr, msg);
#endif // MINWIN_ERROR

extern int  MinwinCreateWindow(const char* title, int x, int y, unsigned int width, unsigned int height, int flags);
extern void MinwinResizeWindow(unsigned int width, unsigned int height);
extern void MinwinPositionWindow(unsigned int x, unsigned int y);
extern void MinwinDestroyWindow();
extern void MinwinSwapBuffers();

typedef void(*MinwinKeyCallback)(int, int);
typedef void(*MinwinButtonCallback)(int, int);
typedef void(*MinwinCursorCallback)(int, int);
typedef void(*MinwinResizeCallback)(unsigned int, unsigned int);

typedef struct {
	MinwinKeyCallback keyCallback;
	MinwinButtonCallback buttonCallback;
	MinwinCursorCallback cursorCallback;
	MinwinResizeCallback resizeCallback;
} MinwinCallbacks;
MinwinCallbacks g_MinwinCallbacks;

extern void MinwinSetKeyCallback(MinwinKeyCallback callback);
extern void MinwinSetButtonCallback(MinwinButtonCallback callback);
extern void MinwinSetCursorCallback(MinwinCursorCallback callback);
extern void MinwinSetResizeCallback(MinwinResizeCallback callback);
extern int  MinwinPollEvents();

// NOTE @LINUX
#ifdef MINWIN_LINUX_IMPLEMENTATION
#include <X11/Xlib.h>
Display* display;
Window window;

#ifdef MINWIN_GL
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glx.h>
GLXContext glXContext;
#endif //MINWIN_GL

int MinwinCreateWindow(const char* title, int x, int y, unsigned int width, unsigned int height, int flags) {
	display = XOpenDisplay(NULL);
	if (display == NULL) {
		 MINWIN_ERROR("Cannot open Xdisplay \n");
		 return 1;
	}

	#ifdef MINWIN_GL
		GLint attributes[] = {
			GLX_RGBA,
			GLX_DEPTH_SIZE,
			24,
			GLX_DOUBLEBUFFER,
			None
		};
		XVisualInfo* visualInfo = glXChooseVisual(display, 0, attributes);
		if(visualInfo == NULL) {
			MINWIN_ERROR("No Approirate visual found\n");
			return 0;
		}

		Colormap colorMap;
		colorMap = XCreateColormap(display, root, visualInfo->visual, AllocNone);
		glXContext = glXCreateContext(display, visualInfo, NULL, GL_TRUE);

		Window root = RootWindow(display, DefaultScreen(display));
		window = XCreateSimpleWindow(display, root, x, y, width, height, 1, 1, 0);
		glXMakeCurrent(display, window, glXContext);
	#elif	//MINWIN_GL
	Window root = RootWindow(display, DefaultScreen(display));
	window = XCreateSimpleWindow(display, root, x, y, width, height, 1, 1, 0);
	XStoreName(display, window, title);
	XSelectInput(display, window, ExposureMask | KeyPressMask | KeyReleaseMask | ButtonReleaseMask);
	XMapWindow(display, window);
#endif
	return 0;
}

#ifdef MINWIN_GL
void MinwinSwapBuffers() {
	glXSwapBuffers(display, window);
}
#endif

int MinwinPollEvents() {
		XEvent event;
		XNextEvent(display, &event);
		switch (event.type) {
		case KeyPress:
			if(g_MinwinCallbacks.keyCallback) g_MinwinCallbacks.keyCallback(event.xkey.state, 1);
			return 1;
		case KeyRelease:
			if(g_MinwinCallbacks.keyCallback) g_MinwinCallbacks.keyCallback(event.xkey.state, 0);
			return 1;
		}
		return 0;
}
void MinwinSetKeyCallback(MinwinKeyCallback callback) {
	g_MinwinCallbacks.keyCallback = callback;
}

void MinwinResizeWindow(unsigned int width, unsigned int height) { XResizeWindow(display, window, width, height); }
void MinwinPositionWindow(unsigned int x, unsigned int y) { XMoveWindow(display, window, x, y); }

void MinwinDestroyWindow() {
	#ifdef MINWIN_GL
	glXMakeCurrent(display, None, NULL);
	glXDestroyContext(display, glXContext);
	#endif
	XDestroyWindow(display, window);
	XCloseDisplay(display);
}

#undef MINWIN_LINUX_IMPLEMENTATION
#endif	// MINWIN_LINUX_IMPLEMENTATION

// NOTE @WINDOWS
#ifdef MINWIN_WINDOWS_IMPLEMENTATION
#undef MINWIN_WINDOWS_IMPLEMENTATION
#endif

#endif	// MINWIN_INCLUDE_GUARD
