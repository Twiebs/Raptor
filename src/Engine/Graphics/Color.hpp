#pragma once

#include <Core/Common.hpp>

struct RGBA8 {
	U8 r;
	U8 g;
	U8 b;
	U8 a;
};

struct Color {
	float r;
	float g;
	float b;
	float a;

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

	friend Color operator*(const Color& left, const float32 scalar) {
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
};
