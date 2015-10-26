#pragma once
#include <Core/types.h>

#define BlendOverlayf(base, blend) 	(base < 0.5 ? (2.0 * base * blend) : (1.0 - 2.0 * (1.0 - base) * (1.0 - blend)))
#define Blend(base, blend, funcf) 		Color(funcf(base.r, blend.r), funcf(base.g, blend.g), funcf(base.b, blend.b), funcf(base.a, blend.a))
#define BlendOverlay(base, blend) 		Blend(base, blend, BlendOverlayf)

struct RGBA32 {

};

struct RGBA8 {
	U8 r, g, b, a;
};

struct Color {
	float r, g, b, a;

	Color() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) { }
	Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) { }
	~Color() { }

	friend Color operator-(const Color& left, const Color& right) {
		Color result;
		result.r = left.r - right.r;
		result.g = left.g - right.g;
		result.b = left.b - right.b;
		result.a = left.a - right.a;
		return result;
	}

	friend Color operator*(const Color& left, const float scalar) {
		Color result;
		result.r = left.r * scalar;
		result.g = left.g * scalar;
		result.b = left.b * scalar;
		result.a = left.a * scalar;
		return result;
	}

	friend Color operator+(const Color& left, const Color& right) {
		Color result;
		result.r = left.r + right.r;
		result.g = left.g + right.g;
		result.b = left.b + right.b;
		result.a = left.a + right.a;
		return result;
	}

public:
    static const Color WHITE;
    static const Color RED;
};



