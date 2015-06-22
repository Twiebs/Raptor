#pragma once

#include <Core/Common.hpp>

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

	Color() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) { }
	Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) { }
	~Color() { }
};
