#pragma once

#include<Application\Window.h>
#include<Core\Engine.h>


enum DisplayMode {
	DISPLAYMODE_WINDOWED,
	DISPLAYMODE_WINDOWED_FULLSCREEN,
	DISPLAYMODE_FULLSCREEN
};

class IApplication {
public:
	Engine* engine;

	IApplication();
	~IApplication();

	virtual void Create(char* title, int width, int height, bool fullscrean) = 0;
	virtual void Start() = 0;
	virtual void Exit() = 0;

	virtual void Resize(int width, int height) = 0;
	virtual int GetWidth() = 0;
	virtual int GetHeight() = 0;

protected:
	int width, height;
	bool running;
};



