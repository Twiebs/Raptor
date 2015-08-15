#ifdef SDL_PLATFORM

#include <Core/Application.hpp>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_image.h>
#undef main

#define GLEW_STATIC
#include <GL/glew.h>
#include <imgui/imgui.h>
#include <Core/Audio.hpp>

SDL_Window* window;
SDL_GLContext context;

int Application::Create(const char* title, uint32 width, uint32 height, bool fullscreen) {
	this->width = width;
	this->height = height;
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
		LOG_ERROR("Unable to initialize SDL:" << SDL_GetError());
		return -1;
	}

	int mixFlags = MIX_INIT_OGG | MIX_INIT_FLAC | MIX_INIT_MP3;
	int mixInitialized = Mix_Init(mixFlags);
	if (mixInitialized & mixFlags != mixFlags) {
		LOG_ERROR("AUDIO: Failed to init required audio library support");
		LOG_ERROR("AUDIO: " << Mix_GetError());
	}
	if (Mix_OpenAudio(AUDIO_FREQUENCY, AUDIO_FORMAT, AUDIO_CHANNELS, AUDIO_CHUNK_SIZE)) {
		LOG_ERROR("AUDIO: Failed to initalize audio context" << Mix_GetError());
	}
//
//	int imgFlags = IMG_INIT_PNG;
//	int imgInitalized = IMG_Init(imgFlags);
//	if (imgInitalized & imgFlags != imgFlags) {
//		LOG_ERROR("IMG: Failed to init required img library support" << IMG_GetError);
//	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);


	SDL_Surface* screen;
	//TODO fullscren does nothing...
	window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
	if (!window)
		LOG_ERROR(SDL_GetError());
	context = SDL_GL_CreateContext(window);

	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		LOG_ERROR("GLEW failed to initialize");
		return -1;
	}

	//Platform was initialized sucuessfuly
	memset(buttonsDown, 0, 6);
	isRunning = true;
	return 0;
}


int Application::Destroy() {
//	IMG_Quit();
	Mix_CloseAudio();
	Mix_Quit();
	SDL_Quit();
	return 0;
}

void Application::BeginFrame() {
	static F64 lastTime = GetTime();
	F64 currentTime = GetTime();
	deltaTime = (currentTime - lastTime) / 1000.0f;
	lastTime = GetTime();
}

void Application::EndFrame() {
	mouseWheel = 0;
	cursorDeltaX = 0;
	cursorDeltaY = 0;
	SDL_GL_SwapWindow(window);
}

float64 Application::GetTime() {
	return (float64)SDL_GetTicks();
}

void Application::SetCursorHidden(bool isHidden) {
	SDL_SetRelativeMouseMode((SDL_bool)isHidden);
}

void GetWindowSize(int* width, int* height) {
	SDL_GetWindowSize(window, width, height);
}

void Application::PollEvents() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {

		case SDL_TEXTINPUT:
		{
			ImGuiIO& io = ImGui::GetIO();
			unsigned int c = event.text.text[0];
			if (c > 0 && c < 0x10000){
				io.AddInputCharacter((unsigned short)c);
			}
		} break;

		case SDL_KEYUP:
		case SDL_KEYDOWN:
		{
			if (keyCallback != nullptr) keyCallback(event.key.keysym.scancode, (event.type == SDL_KEYDOWN));
			keysDown[event.key.keysym.scancode] = (event.type == SDL_KEYDOWN);

			ImGuiIO io = ImGui::GetIO();
			int key = event.key.keysym.sym & ~SDLK_SCANCODE_MASK;
			io.KeysDown[key] = (event.type == SDL_KEYDOWN);
			io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
			io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
			io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
		}	break;

		case SDL_MOUSEMOTION:
			cursorDeltaX = (F64)event.motion.xrel;
			cursorDeltaY = (F64)event.motion.yrel;
			cursorX = (F64)event.motion.x;
			cursorY = (F64)event.motion.y;
			if (cursorPosCallback) cursorPosCallback(cursorDeltaX, cursorDeltaY);
			break;
		case SDL_MOUSEWHEEL:
			mouseWheel = event.wheel.y;
			break;
		case SDL_MOUSEBUTTONDOWN:
			buttonsDown[event.button.button] = true;
			break;
		case SDL_MOUSEBUTTONUP:
			buttonsDown[event.button.button] = false;
			break;

		case SDL_QUIT:
			isRunning = false;
			break;
		}
	}
}

#endif //SDL
