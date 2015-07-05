#pragma once

#include <string>
#include <Graphics/Pixmap.hpp>

int PlatformInit(const char* title, int width, int height, bool fullscreen);
int PlatformShutdown();

void PlatformBeginFrame();
void PlatformEndFrame();

bool PlatformHandleInput();

int PlatformGetWidth();
int PlatformGetHeight();
double PlatformGetDeltaTime();

double PlatformGetCursorX();
double PlatformGetCursorY();
double PlatformGetMouseWheel();
bool PlatformGetKeyDown(int keycode);
bool PlatformGetButtonDown(int button);
void PlatformSetCursorHidden();

Pixmap* PlatformLoadPixmap(std::string& filename);
