#define MINWIN_LINUX_IMPLEMENTATION
#define MINWIN_GL

#include "minwin.h"

void KeyCallback(int keycode, int state) {
	static int resized = 0;
	if(state == 0) return;
	if(!resized) {
		MinwinResizeWindow(250, 250);
		resized = 1;
	} else {
		MinwinResizeWindow(1280, 720);
		resized = 0;
	}
}

int main() {
	MinwinCreateWindow("Minwin Window Created!", 25, 25, 1280, 720, 0);
	MinwinSetKeyCallback(KeyCallback);
	static int running = 1;
	while (running) {
			MinwinPollEvents();

			glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
			MinwinSwapBuffers();
	}
	return 0;
	MinwinDestroyWindow();
}
