#ifndef RAPTOR_PLATFORM_H
#define RAPTOR_PLATFORM_H

#include <Core/types.h>
#include <Core/logging.h>

#include <functional>

#ifdef __WINDOWS__
#define PLATFORM_WINDOWS
#endif



struct CodeModule;
CodeModule LoadModule(const std::string& filename);
#include "platform_windows.h"




struct FileWriteTime {
	U64 lowDateTime;
	U64 highDateTime;
};

bool operator==(const FileWriteTime& a, const FileWriteTime& b);
bool operator!=(const FileWriteTime& a, const FileWriteTime& b);
FileWriteTime GetLastModifedTime (const std::string& filename);
FileWriteTime GetLastWriteTimeForDirectory (const std::string& path);
void ListFilesInDirectory(const std::string& directory, unsigned int level = 0);
void GetFilesInDirectory(const std::string& directory, std::vector<std::string> files, std::vector<std::string>* directories);

int PlatformPopKeypress();

extern "C" int  PlatformCreate(const char* title, int width = 1280, int height = 720, int flags = 0);
void PlatformRun(const std::function<void(double)>&);
extern "C" void PlatformExit();
extern "C" double PlatformGetDeltaTime();

U64 PlatformGetPerformanceCounter();
U64 PlatformGetPerformanceCounterFrequency();

#ifdef PLATFORM_SDL
#include <SDL2/SDL.h>
#undef main
#endif // PLATFORM_SDL

#ifdef PLATFORM_HTML5
#include <SDL/SDL.h>
#undef main
#endif

#if defined(PLATFORM_SDL) || defined(PLATFORM_HTML5)
#define PlatformGetCursorPos(xptr, yptr) SDL_GetMouseState(xptr, yptr)
#define PlatformGetCursorDelta(xptr, yptr) SDL_GetRelativeMouseState(xptr, yptr)
#define PlatformGetKey(keycode) __SDLPlatformGetKey(keycode)
#define PlatformGetButton(button) (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(button))
extern "C" void PlatformGetSize(int* w, int* h);

extern "C" double PlatformGetNow();
extern "C" float PlatformGetNowFloat();


static int __SDLPlatformGetKey(int keycode);
static int __SDLPlatformGetKey(int keycode) {
	static int keycount;
	static const Uint8* keystates = SDL_GetKeyboardState(&keycount);
	assert(keycode < keycount);
	return keystates[keycode];
}
#endif  // defined(PLATFORM_SDL) || defined(__EMSCRIPTEN__)

#ifdef PLATFORM_SDL
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
#define KEY_BACKSPACE	SDLK_BACKSPACE
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
#define KEY_EXECUTE_NOT_USED   116
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

#endif // PLATFORM_SDL

#ifdef PLATFORM_HTML5
#include <SDL/SDL.h>

#define	KEY_UNKNOWN		0
#define	KEY_FIRST		0
#define	KEY_BACKSPACE	8
#define	KEY_TAB			9
#define	KEY_CLEAR		12
#define	KEY_ENTER		13
#define	KEY_PAUSE		19
#define	KEY_ESCAPE		27
#define	KEY_SPACE		32
#define	KEY_EXCLAIM		33
#define	KEY_QUOTEDBL	34
#define	KEY_HASH		35
#define	KEY_DOLLAR		36
#define	KEY_AMPERSAND	38
#define	KEY_QUOTE		39
#define	KEY_LEFTPAREN	40
#define	KEY_RIGHTPAREN	41
#define	KEY_ASTERISK	42
#define	KEY_PLUS		43
#define	KEY_COMMA		44
#define	KEY_MINUS		45
#define	KEY_PERIOD		46
#define	KEY_SLASH		47
#define	KEY_0			48
#define	KEY_1			49
#define	KEY_2			50
#define	KEY_3			51
#define	KEY_4			52
#define	KEY_5			53
#define	KEY_6			54
#define	KEY_7			55
#define	KEY_8			56
#define	KEY_9			57
#define	KEY_COLON		58
#define	KEY_SEMICOLON	59
#define	KEY_LESS		60
#define	KEY_EQUALS		61
#define	KEY_GREATER		62
#define	KEY_QUESTION	63
#define	KEY_AT			64

#define	KEY_LEFTBRACKET	91
#define	KEY_BACKSLASH	92
#define	KEY_RIGHTBRACKET93
#define	KEY_CARET		94
#define	KEY_UNDERSCORE	95
#define	KEY_BACKQUOTE	96
#define	KEY_A			97
#define	KEY_B			98
#define	KEY_C			99
#define	KEY_D			100
#define	KEY_E			101
#define	KEY_F			102
#define	KEY_G			103
#define	KEY_H			104
#define	KEY_I			105
#define	KEY_J			106
#define	KEY_K			107
#define	KEY_L			108
#define	KEY_M			109
#define	KEY_N			110
#define	KEY_O			111
#define	KEY_P			112
#define	KEY_Q			113
#define	KEY_R			114
#define	KEY_S			115
#define	KEY_T			116
#define	KEY_U			117
#define	KEY_V			118
#define	KEY_W			119
#define	KEY_X			120
#define	KEY_Y			121
#define	KEY_Z			122
#define	KEY_DELETE		127

#define	KEY_KP0			256
#define	KEY_KP1		    257
#define	KEY_KP2		    258
#define	KEY_KP3		    259
#define	KEY_KP4		    260
#define	KEY_KP5		    261
#define	KEY_KP6		    262
#define	KEY_KP7		    263
#define	KEY_KP8		    264
#define	KEY_KP9		    265
#define	KEY_KP_PERIOD	266
#define	KEY_KP_DIVIDE	267
#define	KEY_KP_MULTIPLY	268
#define	KEY_KP_MINUS	269
#define	KEY_KP_PLUS		270
#define	KEY_KP_ENTER	271
#define	KEY_KP_EQUALS	272

#define	KEY_UP			273
#define	KEY_DOWN		274
#define	KEY_RIGHT		275
#define	KEY_LEFT		276
#define	KEY_INSERT		277
#define	KEY_HOME		278
#define	KEY_END			279
#define	KEY_PAGEUP		280
#define	KEY_PAGEDOWN	281

#define	KEY_F1			282
#define	KEY_F2			283
#define	KEY_F3			284
#define	KEY_F4			285
#define	KEY_F5			286
#define	KEY_F6			287
#define	KEY_F7			288
#define	KEY_F8			289
#define	KEY_F9			290
#define	KEY_F10		    291
#define	KEY_F11		    292
#define	KEY_F12		    293
#define	KEY_F13		    294
#define	KEY_F14		    295
#define	KEY_F15		    296

#define	KEY_NUMLOCK		300
#define	KEY_CAPSLOCK	301
#define	KEY_SCROLLOCK	302
#define	KEY_RSHIFT		303
#define	KEY_LSHIFT		304
#define	KEY_RCTRL		305
#define	KEY_LCTRL		306
#define	KEY_RALT		307
#define	KEY_LALT		308
#define	KEY_RMETA		309
#define	KEY_LMETA		310
#define	KEY_LSUPER		311	/**< Left "Windows" key */
#define	KEY_RSUPER		312	/**< Right "Windows" key */
#define	KEY_MODE		313	/**< "Alt Gr" key */
#define	KEY_COMPOSE		314	/**< Multi-key compose key */

#define	KEY_HELP		315
#define	KEY_PRINT		316
#define	KEY_SYSREQ		317
#define	KEY_BREAK		318
#define	KEY_MENU		319
#define	KEY_POWER		320		/**< Power Macintosh power key */
#define	KEY_EURO		321		/**< Some european keyboards */
#define	KEY_UNDO		322		/**< Atari keyboard has Undo */

#define MOUSE_BUTTON_LEFT 	1
#define MOUSE_BUTTON_MIDDLE 2
#define MOUSE_BUTTON_RIGHT  3
#define MOUSE_WHEEL_UP		4
#define MOUSE_WHEEL_DOWN  	5
#endif	// __EMSCRIPTEN__

#endif	// RAPTOR_PLATFORM_H
