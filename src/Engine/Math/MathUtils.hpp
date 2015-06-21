#pragma once

#include <cmath>
#include <Core/Common.hpp>

#define PI 3.14159265358979323846

namespace MathUtils {

	float Clamp(float value, float min, float max);

	bool EpsilonEquals(float a, float b, float epsilon = 0.1);

	inline static uint32 NextPowerOfTwo(uint32 n) {
		n--;
		n |= n >> 1;
		n |= n >> 2;
		n |= n >> 4;
		n |= n >> 8;
		n |= n >> 16;
		n++;
		return n;
	}

	inline static float Radians(float degrees) {
		return (PI / 180.0f) * degrees;
	}

	inline static float Max(float value, float max) {
		return value > max ? value : max;
	}

};
