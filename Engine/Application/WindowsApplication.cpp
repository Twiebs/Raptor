#include "WindowsApplication.h"


WindowsApplication::WindowsApplication() {
}


WindowsApplication::~WindowsApplication() {
}

int WindowsApplication::Init(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdline, int showCode) {
	//TODO windows platform layer
	return 0;
}

void WindowsApplication::MainWindowCallback(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {
	LRESULT result = 0;
	switch (message) {
	case WM_CLOSE:
		Exit();
		break;
	case WM_ACTIVATEAPP:
		//
		break;
	case WM_DESTROY:
		//Window was destroyed forcefully... display error
		break;

	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	case WM_KEYDOWN:
	case WM_KEYUP:
		//TODO Input handling
		break;
	case WM_PAINT:
		//OpenGL renderering here.
		break;
	default:
		break;
	}

}