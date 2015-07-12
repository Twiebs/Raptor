#pragma once
#include <cmath>
#include <Core/Common.hpp>

#define PI32 3.141592653589793238462643383f
#define PI64 3.1415926535897932384626433832795028842
#define TAU32 PI32 * 2
#define TAU64 PI64 * 2

#define RADIANS(x) (PI32 / 180.f) * x

namespace MathUtils {

	inline static F32 Clamp(F32 value, F32 min, F32 max) {
		F32 clamped = value;
		if (value > max)
			clamped = max;
		else if (value < min)
			clamped = min;
		return clamped;
	}

	inline static bool EpsilonEquals(F32 a, F32 b, F32 epsilon = 0.1) {
		F32 difference = std::abs(a - b);
		if (difference < epsilon)
			return true;
		return false;
	}

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
		return (PI32 / 180.0f) * degrees;
	}

	inline static float Max(float value, float max) {
		return value > max ? value : max;
	}

	inline static F32 Min(F32 value, F32 min) {
		return value < min ? value : min;
	}

	inline static int FastFloor(float64 x) {
		int xi = (int)x;
		return x < xi ? xi - 1 : xi;
	}

};
