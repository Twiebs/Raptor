#include "MathUtils.h"

float MathUtils::Clamp(float value, float min, float max) {
	float clamped = value;
	if (value > max)
		clamped = max;
	else if (value < min)
		clamped = min;
	return clamped;
}

bool MathUtils::EpsilonEquals(float a, float b, float epsilon) {
	float difference = abs(a - b);
	if (difference < epsilon)
		return true;
	return false;
}