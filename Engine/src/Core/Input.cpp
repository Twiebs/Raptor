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
	if (listeners.size() == 0) listeners = std::vector<IInputListener*>() ;
	listeners.push_back(listener);
}

void IApplicationInput::OnKeyDown(int keycode, int mods) {
	keysDown[keycode] = true;

	//Notifiy all registered input listenes that a key was pressed
	for each(IInputListener* listener in listeners) {
		if (listener->OnKeyDown(keycode, mods)) {
			//if the listener returns true the event was handled and we can break
			break;
		}
	}
}

void IApplicationInput::OnKeyUp(int keycode) {
	keysDown[keycode] = false;
}

void IApplicationInput::OnCursorPos(double xpos, double ypos) {
	this->cursorX = xpos;
	this->cursorY = ypos;
}

