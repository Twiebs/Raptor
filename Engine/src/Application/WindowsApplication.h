#pragma once

#include<Application\OpenGLApplication.h>
#include<Windows.h>

class WindowsApplication : public OpenGLApplication{
public:
	WindowsApplication();
	~WindowsApplication();

	int Init(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdline, int showCode);
	void MainWindowCallback(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
private:
	HINSTANCE hInstance;
};

