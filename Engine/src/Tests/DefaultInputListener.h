#pragma once

#include<Core\Engine.h>
#include<Core\Input.h>

class DefaultInputListener : public IInputListener{
public:
	DefaultInputListener(Engine* engine, BaseScene* scene);
	~DefaultInputListener();

	bool OnKeyDown(int keycode, int mods) override;
	bool OnCursorPos(double xpos, double ypos) override;

private:
	Engine* engine;
	BaseScene* scene;
};

