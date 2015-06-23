#pragma once

int PlatformInit(const char* title, int width, int height, bool fullscreen);
int PlatformShutdown();

void PlatformBeginFrame();
void PlatformEndFrame();

bool PlatformHandleInput();
