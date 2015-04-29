
#include "DefaultInputListener.h"

DefaultInputListener::DefaultInputListener(Engine* engine, BaseScene* scene) {
	this->engine = engine;
	this->scene = scene;
}

DefaultInputListener::~DefaultInputListener() {
}

bool DefaultInputListener::OnKeyDown(int keycode, int mods) {
	switch (keycode) {
	case INPUT_KEY_ESCAPE:
		engine->Exit();
		return true;
	case INPUT_KEY_F3:
		scene->renderer->ToggleWireframeEnabled();
		return true;
	default:
		return false;
	}
}


bool DefaultInputListener::OnCursorPos(double xpos, double ypos) {
	return false;
}
