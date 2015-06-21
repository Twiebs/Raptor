#pragma once

//Interface for interacting with the application layer to recieve input events
//Event functions return true if the even was handeled by the listener.
//If true is returned no subsequent listners will recieve the event.
class IInputListener {
public:
	IInputListener() { }
	~IInputListener() { }

	//Called when the application recieves an input event from the platform layer
	//Returns true if the event was handeled by this input listner.
	virtual bool OnKeyEvent(int keycode, bool isPressed, int mods) {
		return false;
	}

	//Called when the application recieves a mouse button event from the platform
	//Returns true if the event was handled by the inputlistener
	virtual bool OnMouseButtonEvent(int button, double cursorX, double cursorY, bool isPressed, int mods) {
		return false;
	}

	//Called when the cursor position changes
	//args are the absolute coordinates of the cursor in the window
	virtual bool OnCursorPosEvent(double xPos, double yPos) {
		return false;
	}
};

