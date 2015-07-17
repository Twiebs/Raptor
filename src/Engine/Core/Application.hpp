#pragma once

#include <vector>
#include <Core/Common.hpp>
#include <Core/InputListener.hpp>

#define AUDIO_STEREO 2
#define AUDIO_MONO 1

#ifdef SDL
#define KEY_UNKOWN 0
#define KEY_A 4
#define KEY_B 5
#define KEY_C 6
#define KEY_D 7
#define KEY_E 8
#define KEY_F 9
#define KEY_G 10
#define KEY_H 11
#define KEY_I 12
#define KEY_J 13
#define KEY_K 14
#define KEY_L 15
#define KEY_M 16
#define KEY_N 17
#define KEY_O 18
#define KEY_P 19
#define KEY_Q 20
#define KEY_R 21
#define KEY_S 22
#define KEY_T 23
#define KEY_U 24
#define KEY_V 25
#define KEY_W 26
#define KEY_X 27
#define KEY_Y 28
#define KEY_Z 29

#define KEY_1 30
#define KEY_2 31
#define KEY_3 32
#define KEY_4 33
#define KEY_5 34
#define KEY_6 35
#define KEY_7 36
#define KEY_8 37
#define KEY_9 38
#define KEY_0 39

#define KEY_RETURN		40
#define KEY_ENTER		40
#define KEY_ESCAPE		41
#define KEY_BACKSPACE	42
#define KEY_TAB			43
#define KEY_SPACE		44

#define KEY_MINUS	  45
#define KEY_EQUALS   46
#define KEY_LEFTBRACKET   47
#define KEY_RIGHTBRACKET   48
#define KEY_BACKSLASH   49
#define KEY_NONUSHASH   50
#define KEY_SEMICOLON   51
#define KEY_APOSTROPHE   52
#define KEY_GRAVE   53
#define KEY_COMMA   54
#define KEY_PERIOD   55
#define KEY_SLASH   56

#define KEY_CAPSLOCK   57

#define KEY_F1   58
#define KEY_F2   59
#define KEY_F3   60
#define KEY_F4   61
#define KEY_F5   62
#define KEY_F6   63
#define KEY_F7   64
#define KEY_F8   65
#define KEY_F9   66
#define KEY_F10   67
#define KEY_F11   68
#define KEY_F12   69

#define KEY_PRINTSCREEN   70
#define KEY_SCROLLLOCK   71
#define KEY_PAUSE   72
#define KEY_INSERT   73
#define KEY_HOME   74
#define KEY_PAGEUP   75
#define KEY_DELETE   76
#define KEY_END   77
#define KEY_PAGEDOWN   78
#define KEY_RIGHT   79
#define KEY_LEFT   80
#define KEY_DOWN   81
#define KEY_UP   82

#define KEY_NUMLOCKCLEAR   83
#define KEY_KP_DIVIDE   84
#define KEY_KP_MULTIPLY   85
#define KEY_KP_MINUS   86
#define KEY_KP_PLUS   87
#define KEY_KP_ENTER   88
#define KEY_KP_1   89
#define KEY_KP_2   90
#define KEY_KP_3   91
#define KEY_KP_4   92
#define KEY_KP_5   93
#define KEY_KP_6   94
#define KEY_KP_7   95
#define KEY_KP_8   96
#define KEY_KP_9   97
#define KEY_KP_0   98
#define KEY_KP_PERIOD   99

#define KEY_NONUSBACKSLASH   100
#define KEY_APPLICATION   101
#define KEY_POWER   102
#define KEY_KP_EQUALS   103
#define KEY_F13   104
#define KEY_F14   105
#define KEY_F15   106
#define KEY_F16   107
#define KEY_F17   108
#define KEY_F18   109
#define KEY_F19   110
#define KEY_F20   111
#define KEY_F21   112
#define KEY_F22   113
#define KEY_F23   114
#define KEY_F24   115
#define KEY_EXECUTE   116
#define KEY_HELP   117
#define KEY_MENU   118
#define KEY_SELECT   119
#define KEY_STOP   120
#define KEY_AGAIN   121    /**< redo */
#define KEY_UNDO   122
#define KEY_CUT   123
#define KEY_COPY   124
#define KEY_PASTE   125
#define KEY_FIND   126
#define KEY_MUTE   127
#define KEY_VOLUMEUP   128
#define KEY_VOLUMEDOWN   129

#define KEY_KP_COMMA   133
#define KEY_KP_EQUALSAS400   134

#define KEY_LCTRL   224
#define KEY_LSHIFT   225
#define KEY_LALT   226
#define KEY_LGUI   227
#define KEY_RCTRL   228
#define KEY_RSHIFT   229
#define KEY_RALT   230
#define KEY_RGUI   231

#define MOUSE_BUTTON_LEFT 	1
#define MOUSE_BUTTON_MIDDLE 2
#define MOUSE_BUTTON_RIGHT  3
#define MOUSE_WHEEL_UP		4
#define MOUSE_WHEEL_DOWN  	5
#endif

typedef void (*InputKeyCallback)(int, bool);

class Application {
public:
	bool isRunning = false;


	Application(const char* tile, U32 width = 1280, U32 height = 720, bool fullscreen = false) {
		Create(tile, width, height, fullscreen);
		memset(keysDown, false, 1024);
	}

	Application() {
//		memset(keysDown, false, 1024);
	}
	~Application() {
		Destroy();
	}

	//Platform specific overrides
	int Create(const char* tile, U32 width = 1280, U32 height = 720, bool fullscreen = false);

	void Run(void (*mainLoop)(Application*)) {
		while(isRunning) {
			BeginFrame();
			PollEvents();
			mainLoop(this);
			EndFrame();
		}
	}

	int Destroy();
	void BeginFrame();
	void EndFrame();
	void PollEvents();
	F64 GetTime();

	inline bool IsKeyDown(U32 keycode) { return keysDown[keycode]; }
	inline void SetKeyCallback(InputKeyCallback kb) { this->keyCallback = kb; }
	inline bool IsButtonDown(U32 buttoncode) { return buttonsDown[buttoncode]; }

	inline F64 GetCursorX() { return cursorX; }
	inline F64 GetCursorY() { return cursorY; }
	inline F64 GetMouseWheel() { return mouseWheel; }

	inline F32 GetWidth() { return width; }
	inline F32 GetHeight() { return height; }
	F64 GetDeltaTime() { return deltaTime; }

private:
	F32 width = 0, height = 0;
	F64 deltaTime = 1.0f / 60.0f;	//Set to initial value to insure the first frame of the application has a deltaTime
	bool keysDown[1024];
	bool buttonsDown[6];
	F64 cursorX = 0, cursorY = 0;
	F64 mouseWheel = 0;
	InputKeyCallback keyCallback = nullptr;
};