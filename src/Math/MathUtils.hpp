#pragma once

#include <cmath>
#include <cfloat>
#include <Core/Common.hpp>

#define PI32 3.141592653589793238462643383f
#define PI64 3.1415926535897932384626433832795028842
#define TAU32 PI32 * 2
#define TAU64 PI64 * 2

#define RADIANS(x) (PI32 / 180.f) * x

inline F32 Lerp (F32 from, F32 to, F32 t) {
	auto result = ((1.0f - t) * from) + (t*to);
	return result;
}

inline float Clamp (float value, float min, float max) {
	if (value > max) return max;
	if (value < min) return min;
	return value;
}

inline float SmoothStep (float a, float b, float x) {
	x = Clamp((x - a)/(b - a), 0.0f, 1.0f);
	return x*x*(3 - 2*x);
}

namespace MathUtils {

	inline static F32 Clamp(F32 value, F32 min, F32 max) {
		F32 clamped = value;
		if (value > max)
			clamped = max;
		else if (value < min)
			clamped = min;
		return clamped;
	}


	inline static bool EpsilonEquals(F32 a, F32 b, F32 epsilon = FLT_EPSILON) {
		auto result = (std::abs(a - b) < epsilon);
		return result;
	}
	inline static bool EpsilonEquals(F64 a, F64 b, F64 epsilon = DBL_EPSILON) {
		auto result = (std::abs(a - b) < epsilon);
		return result;
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
