#pragma once

#include <string>
#include <Graphics/Pixmap.hpp>

int PlatformInit(const char* title, int width, int height, bool fullscreen);
int PlatformShutdown();

void PlatformBeginFrame();
void PlatformEndFrame();

bool PlatformHandleInput();

Pixmap* PlatformLoadPixmap(std::string& filename);
