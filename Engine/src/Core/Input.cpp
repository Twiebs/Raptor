#include "Input.h"

IInputListener::IInputListener() {
}


IInputListener::~IInputListener() {
}


IApplicationInput::IApplicationInput() {

}

IApplicationInput::~IApplicationInput() {

}


void IApplicationInput::AddListener(IInputListener* listener) {
	listeners.push_back(listener);
}

void IApplicationInput::FireKeyEvent(int keycode, bool isPressed, int mods) {
	keysDown[keycode] = isPressed;

	//Notifiy all registered input listenes that a key was pressed
	for each(IInputListener* listener in listeners) {
		if (listener->OnKeyEvent(keycode, isPressed, mods)) {
			//if the listener returns true the event was handled and we can break
			break;
		}
	}
}

void IApplicationInput::FireCursorPosEvent(double xpos, double ypos) {
	this->cursorX = xpos;
	this->cursorY = ypos;
	for each(IInputListener* listener in listeners) {
		if (listener->OnCursorPosEvent(xpos, ypos))
			break;
	}
}

