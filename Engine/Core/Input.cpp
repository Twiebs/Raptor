#include "Input.h"


//Input Listner
IInputListener::IInputListener() {
}


IInputListener::~IInputListener() {
}

bool IInputListener::OnKeyEvent(int keycode, bool isPressed, int mods) {
	return false;
}
bool IInputListener::OnMouseButtonEvent(int button, bool isPressed, int mods){
	return false;
}
bool IInputListener::OnCursorPosEvent(double xpos, double ypos) {
	return false;
}




IApplicationInput::IApplicationInput() {

}

IApplicationInput::~IApplicationInput() {

}


void IApplicationInput::AddListener(IInputListener* listener) {
	listeners.push_back(listener);
}

bool IApplicationInput::RemoveListener(IInputListener* listener){
	for (int i = 0; i < listeners.size(); i++){
		if (listeners[i] == listener){
			listeners.erase(listeners.begin() + i);
			return true;
		}
	}
	return false;
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

void IApplicationInput::FireMouseButtonEvent(int button, bool isPressed, int mods){
	for each(IInputListener* listener in listeners){
		listener->OnMouseButtonEvent(button, isPressed, mods);
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

