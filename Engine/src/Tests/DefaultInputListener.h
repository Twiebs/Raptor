#pragma once

#include<Core\Engine.h>
#include<Core\Input.h>
#include "SpaceScene.h"

class DefaultInputListener : public IInputListener{
public:
	friend class BaseScene;
	DefaultInputListener(Engine* engine, BaseScene* scene);
	~DefaultInputListener();

	bool OnKeyEvent(int keycode, bool isPressed, int mods) override;
	bool OnCursorPosEvent(double xpos, double ypos) override;

private:
	Engine* engine;
	BaseScene* scene;
};

