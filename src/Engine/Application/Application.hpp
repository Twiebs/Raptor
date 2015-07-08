#pragma once

#include <vector>
#include <Core/Common.hpp>
#include <Core/InputListener.hpp>

class Application {
public:
	bool isRunning = false;

	Application() { }
	~Application() { }

	//Platform specific overrides
	int Create(const char* tile, uint32 width, uint32 height, bool fullscreen);
	int Destroy();
	void BeginFrame();
	void EndFrame();
	void PollEvents();
	float64 GetTime();
	//========================

	inline bool IsKeyDown(uint32 keycode) { return keysDown[keycode]; }
	inline bool IsButtonDown(uint32 buttoncode) { return buttonsDown[buttoncode]; }

	inline float64 GetCursorX() { return cursorX; }
	inline float64 GetCursorY() { return cursorY; }
	inline float64 GetMouseWheel() { return mouseWheel; }

	inline float32 GetWidth() { return width; }
	inline float32 GetHeight() { return height; }
	float64 GetDeltaTime() { return deltaTime; }

	void AddListener(InputListener* listener) {
		listeners.push_back(listener);
	}

	bool RemoveListener(InputListener* listener) {
		for (int i = 0; i < listeners.size(); i++){
			if (listeners[i] == listener){
				listeners.erase(listeners.begin() + i);
				return true;
			}
		}
		return false;
	}
	//Called by the platform notifies all registered listeners about the event
	void FireKeyEvent(int keycode, bool isPressed, int mods) {
		for(auto listener : listeners) {
			if(listener->OnKeyEvent(keycode, isPressed, mods)) {
				return;
			}
		}
	}
	void FireMouseButtonEvent(int button, bool isPressed, int mods) {
		for(auto listener : listeners) {
			if(listener->OnMouseButtonEvent(button, cursorX, cursorY, isPressed, mods)) {
				return;
			}
		}
	}
	void FireCursorPosEvent(double xpos, double ypos) {
		for(auto listener : listeners) {
			if(listener->OnCursorPosEvent(xpos, ypos)) {
				return;
			}
		}
	}

private:
	float32 width = 0, height = 0;
	//Set to initial value to insure the first frame of the application has a deltaTime
	float64 deltaTime = 1.0f/60.0f;

	std::vector<InputListener*> listeners;
	bool keysDown[1024];
	bool buttonsDown[6];
	float64 cursorX = 0, cursorY = 0;
	float64 mouseWheel = 0;
};

#ifdef SDL
#include <Application/SDLApplication.cpp>
#endif

extern Application app;
