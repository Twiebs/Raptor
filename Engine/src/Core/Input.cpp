#include "Input.h"

IInputListener::IInputListener() {
}


IInputListener::~IInputListener() {
}


InputProcessor::InputProcessor() {

}

InputProcessor::~InputProcessor() {

}


void InputProcessor::AddListener(IInputListener* listener) {
	listeners.push_back(listener);
}

void InputProcessor::OnKeyDown(int keycode, int mods) {
	keysDown[keycode] = true;

	//Notifiy all registered input listenes that a key was pressed
	for each(IInputListener* listener in listeners) {
		if (listener->OnKeyDown(keycode, mods)) {
			//if the listener returns true the event was handled and we can break
			break;
		}
	}
}

void InputProcessor::OnKeyUp(int keycode) {
	keysDown[keycode] = false;
}

void InputProcessor::OnCursorPos(double xpos, double ypos) {
	this->cursorX = xpos;
	this->cursorY = ypos;
}

