#pragma once

#include <string>
#include <memory>

#include <Core/InputService.hpp>

enum DisplayMode {
	DISPLAYMODE_WINDOWED,
	DISPLAYMODE_WINDOWED_FULLSCREEN,
	DISPLAYMODE_FULLSCREEN
};

class IApplication {
public:
	IApplication() { }
	~IApplication() { }

	virtual void Create(std::string title, int width, int height, bool fullscrean) = 0;

	virtual void Resize(int width, int height) = 0;
	virtual int GetWidth() const = 0;
	virtual int GetHeight() const = 0;

	virtual double GetTime() = 0;

	virtual bool ShouldClose() = 0;

	virtual void BeginFrame() = 0;
	virtual void EndFrame() = 0;

	virtual void SetDisplayMode(DisplayMode mode) = 0;
	virtual void SetCursorHidden(bool isHidden) = 0;

	virtual void RegisterInputService(InputService* input) = 0;

protected:
	int width, height;
	bool shouldClose = false;


};
