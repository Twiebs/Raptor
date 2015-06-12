#pragma once

#include<Application\IApplicationStartable.h>
#include<Core\Input.h>

namespace Raptor {

}

enum DisplayMode {
	DISPLAYMODE_WINDOWED,
	DISPLAYMODE_WINDOWED_FULLSCREEN,
	DISPLAYMODE_FULLSCREEN
};

class IApplication : public IApplicationInput{
public:
	IApplication();
	~IApplication();

	virtual void Create(char* title, int width, int height, bool fullscrean) = 0;
	virtual void Start() = 0;
	virtual void Attach(IApplicationStartable* startable) = 0;
	virtual void Exit() = 0;

	virtual void Resize(int width, int height) = 0;
	virtual int GetWidth() const = 0;
	virtual int GetHeight() const = 0;

	virtual void SetDisplayMode(DisplayMode mode) = 0;
	virtual void SetCursorHidden(bool isHidden) = 0;

protected:
	int width, height;
	bool running;
};



