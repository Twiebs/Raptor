#include "InputService.hpp"



InputService::InputService() {

}

InputService::~InputService() {

}

void InputService::AddListener(IInputListener* listener) {
	listeners.push_back(listener);
}

bool InputService::RemoveListener(IInputListener* listener){
	for (int i = 0; i < listeners.size(); i++){
		if (listeners[i] == listener){
			listeners.erase(listeners.begin() + i);
			return true;
		}
	}
	return false;
}

void InputService::FireKeyEvent(int keycode, bool isPressed, int mods) {
	keysDown[keycode] = isPressed;

	//Notifiy all registered input listenes that a key was pressed
	for(IInputListener* listener : listeners) {
		if (listener->OnKeyEvent(keycode, isPressed, mods)) {
			//if the listener returns true the event was handled and we can break
			break;
		}
	}
}

void InputService::FireMouseButtonEvent(int button, bool isPressed, int mods){
	for(IInputListener* listener : listeners){
		listener->OnMouseButtonEvent(button, cursorX, cursorY, isPressed, mods);
	}
}


void InputService::FireCursorPosEvent(double xpos, double ypos) {
	this->cursorX = xpos;
	this->cursorY = ypos;
	for(IInputListener* listener : listeners) {
		if (listener->OnCursorPosEvent(xpos, ypos))
			break;
	}
}
