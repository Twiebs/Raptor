
#include "DefaultInputListener.h"

DefaultInputListener::DefaultInputListener(Engine* engine, BaseScene* scene) {
	this->engine = engine;
	this->scene = scene;
}

DefaultInputListener::~DefaultInputListener() {
}

bool DefaultInputListener::OnKeyEvent(int keycode, bool isPressed, int mods) {
	//Not intrested in key releases atm
	if (!isPressed) {
		return false;
	}

	switch (keycode) {
	case INPUT_KEY_ESCAPE:
		engine->GetApp()->Exit();
		return true;

	case INPUT_KEY_F3:
		scene->renderer.ToggleWireframeEnabled();
		return true;

	case INPUT_KEY_GRAVE_ACCENT:
		scene->drawDeveloperCanvas = !scene->drawDeveloperCanvas;
		return true;
	}

	return false;
}


bool DefaultInputListener::OnCursorPosEvent(double xpos, double ypos) {
	return false;
}
