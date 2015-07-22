/*
	===========================================================================================
	Minnow_Window - Ultra minimial, cross plaform, single file, window creation library;
	Version: 0.1
	Author: Torin Wiebelt 2015
	Licence: This file is in the public domain.  No Warranty is implied.  Use at your own risk.
		The author is intrested in hearing how is work is being used.
		If you like you can send him an email torinwiebelt@gmail.com
	 (Note Author is unsure why he is speaking in the 3rd person)
  ===========================================================================================
	This project lives at http://github.com/Twiebs/Minnow

	Usage:
		- #define MINDOW_IMPLEMENTATION in one C or C++ file to create the implementation of the libarary
				#include ...
				#include ...
				#define MINDOW_IMPLEMENTATION
				#include "minwin.h"
				By default this will generate an implementation for the current platform you are compiling on.
				if you want to compile with a different implementation use:
					-  MINDOW_LINUX_IMPLEMENTATION
					-  MINDOW_WINDOWS_IMPLEMENTATION
					-  MINDOW_APPLE_IMPLEMENTATION
					-  MINDOW_ANDROID_IMPLEMENTATION

	Configuration
		- #define MINDOW_ERROR(msg) to use custom error loging otherwise stdio.h is inlcuded
				and errors are logged using fprintf(stderr, msg);
*/

// ===============
//		Header
//================
#ifndef MINDOW_INCLUDE_GUARD
#define MINDOW_INCLUDE_GUARD

#ifndef MINDOW_ERROR
#include <stdio.h>
#define MINDOW_ERROR(msg) fprintf(stderr, msg)
#define MINDOW_TEST_ERROR(msg, ...) fprintf(stderr, msg, __VA_ARGS__)
#endif // MINDOW_ERROR

#define MINDOW_CALLBACK_PREFIX MINDOW_FUNCTION_PREFIX
#define MINDOW_FUNCTION(name) MINDOW_FUNCTION_INTERNAL(Mindow, name)
#define MINDOW_FUNCTION_INTERNAL(prefix, name) prefix ## name
#define MINDOW_CALLBACK(name) Mindow ## name

extern int  MINDOW_FUNCTION(CreateWindow)(const char* title, int x, int y, unsigned int width, unsigned int height, int flags);
extern void MINDOW_FUNCTION(ResizeWindow)(unsigned int width, unsigned int height);
extern void MINDOW_FUNCTION(PositionWindow)(unsigned int x, unsigned int y);
extern void MINDOW_FUNCTION(DestroyWindow)();
extern void MINDOW_FUNCTION(SwapBuffers)();

typedef void(*MINDOW_CALLBACK(KeyCallback))(int, int);
typedef void(*MINDOW_CALLBACK(ButtonCallback))(int, int);
typedef void(*MINDOW_CALLBACK(CursorCallback))(int, int);
typedef void(*MINDOW_CALLBACK(ResizeCallback))(unsigned int, unsigned int);

extern void MINDOW_FUNCTION(SetKeyCallback)(MINDOW_CALLBACK(KeyCallback) 			 callback);
extern void MINDOW_FUNCTION(SetButtonCallback)(MINDOW_CALLBACK(ButtonCallback) callback);
extern void MINDOW_FUNCTION(SetCursorCallback)(MINDOW_CALLBACK(CursorCallback) callback);
extern void MINDOW_FUNCTION(SetResizeCallback)(MINDOW_CALLBACK(ResizeCallback) callback);
extern int  MINDOW_FUNCTION(PollEvents)();

typedef struct {
	MINDOW_CALLBACK(KeyCallback) 		key;
	MINDOW_CALLBACK(ButtonCallback) button;
	MINDOW_CALLBACK(CursorCallback) cursor;
	MINDOW_CALLBACK(ResizeCallback) resize;
} MINDOW_CALLBACK(Callbacks);

// ==================
//			Linux
// ==================
#ifdef MINDOW_LINUX_IMPLEMENTATION
#undef MINDOW_LINUX_IMPLEMENTATION
#include <X11/Xlib.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glx.h>

typedef struct {
	Display* display;
	Window window;
	GLXContext glXContext;
	MindowCallbacks callbacks;
} MindowContext;

MindowContext* MindowGetContext() {
	static MindowContext context;
	return &context;
}

int MINDOW_FUNCTION(CreateWindow)(const char* title, int x, int y, unsigned int width, unsigned int height, int flags) {
	MindowContext* context = MindowGetContext();
	context->display = XOpenDisplay(NULL);
	if (context->display == NULL) {
		 MINDOW_ERROR("Cannot open Xdisplay \n");
		 return 1;
	}

	// Get a matching FB config
  static int visual_attribs[] = {
      GLX_X_RENDERABLE    , True,
      GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
      GLX_RENDER_TYPE     , GLX_RGBA_BIT,
      GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
      GLX_RED_SIZE        , 8,
      GLX_GREEN_SIZE      , 8,
      GLX_BLUE_SIZE       , 8,
      GLX_ALPHA_SIZE      , 8,
      GLX_DEPTH_SIZE      , 24,
      GLX_STENCIL_SIZE    , 8,
      GLX_DOUBLEBUFFER    , True,
      //GLX_SAMPLE_BUFFERS  , 1,
      //GLX_SAMPLES         , 4,
      None
  };

	int attributes[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };

	int glxMajor, glxMinor;
	int sucuess = glXQueryVersion(context->display, &glxMajor, &glxMinor);
	if(!sucuess || ((glxMajor == 1) && glxMinor < 3) || glxMajor < 1) {
		MINDOW_TEST_ERROR("Mindow does not currently support GLX version %d, %d!", glxMajor, glxMinor);
	}

	Window root = RootWindow(context->display, DefaultScreen(context->display));
	XVisualInfo* visualInfo = glXChooseVisual(context->display, 0, attributes);
	if(visualInfo == NULL) {
		MINDOW_ERROR("No Approirate visual found\n");
		return 0;
	}
	Colormap colorMap;
	colorMap = XCreateColormap(context->display, root, visualInfo->visual, AllocNone);
	XSetWindowAttributes swa;
	swa.colormap = colorMap;
	swa.event_mask = ExposureMask | KeyPressMask;
	context->window = XCreateWindow(context->display, root, x, y, width, height, 0, visualInfo->depth, InputOutput, visualInfo->visual, CWColormap | CWEventMask, &swa);
	XStoreName(context->display, context->window, title);
	context->glXContext = glXCreateContext(context->display, visualInfo, NULL, GL_TRUE);
	glXMakeCurrent(context->display, context->window, context->glXContext);
	glViewport(0, 0, width, height);
	XMapWindow(context->display, context->window);
	return 0;
}


void MINDOW_FUNCTION(SwapBuffers)() {
	MindowContext* context = MindowGetContext();
	glXSwapBuffers(context->display, context->window);
}

int MINDOW_FUNCTION(PollEvents)() {
	MindowContext* context = MindowGetContext();
		XEvent event;
		XNextEvent(context->display, &event);
		switch (event.type) {
		case Expose:
			return 1;
		case KeyPress:
			if(context->callbacks.key) context->callbacks.key(event.xkey.state, 1);
			return 1;
		case KeyRelease:
			if(context->callbacks.key) context->callbacks.key(event.xkey.state, 0);
			return 1;
		}
		return 0;
}

void MINDOW_FUNCTION(SetKeyCallback)(MINDOW_CALLBACK(KeyCallback) callback) {
	MindowContext* context = MindowGetContext();
	context->callbacks.key = callback;
}

void MINDOW_FUNCTION(ResizeWindow)(unsigned int width, unsigned int height) {
	MindowContext* context = MindowGetContext();
	 XResizeWindow(context->display, context->window, width, height);
 }
void MINDOW_FUNCTION(PositionWindow)(unsigned int x, unsigned int y) {
	MindowContext* context = MindowGetContext();
 	XMoveWindow(context->display, context->window, x, y);
 }

void MINDOW_FUNCTION(DestroyWindow)() {
	MindowContext* context = MindowGetContext();
	glXMakeCurrent(context->display, None, NULL);
	glXDestroyContext(context->display, context->glXContext);
	XDestroyWindow(context->display, context->window);
	XCloseDisplay(context->display);
}
#endif	// MINDOW_LINUX_IMPLEMENTATION


// ================
//		 Windows
// ================
#ifdef MINDOW_WINDOWS_IMPLEMENTATION
#undef MINDOW_WINDOWS_IMPLEMENTATION
#endif

#endif	// MINDOW_INCLUDE_GUARD
