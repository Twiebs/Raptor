#pragma once

#include<Engine/Engine.h>

struct RGBA8 {
	uint8 r;
	uint8 g;
	uint8 b;
	uint8 a;
};

class Color {
public:
	float r;
	float g;
	float b;
	float a;

	Color();
	Color(float r, float g, float b, float a);
	~Color();
};

