#include "MathUtils.h"

Matrix4 MathUtils::PerspectiveProjection(const float fov, const float ar, const float zNear, const float zFar) {
	Matrix4 matrix;
	const float zRange = zNear - zFar;
	const float tanHalfFOV = tanf(ToRadian(fov / 2.0));

	matrix.m[0][0] = 1.0f / (tanHalfFOV * ar);
	matrix.m[0][1] = 0.0f;
	matrix.m[0][2] = 0.0f;
	matrix.m[0][3] = 0.0f;

	matrix.m[1][0] = 0.0f;
	matrix.m[1][1] = 1.0f / (tanHalfFOV * ar);
	matrix.m[1][2] = 0.0f;
	matrix.m[1][3] = 0.0f;

	matrix.m[2][0] = 0.0f;
	matrix.m[2][1] = 0.0f;
	matrix.m[2][2] = (-zNear - zFar) / zRange;
	matrix.m[2][3] = (2.0f * zFar * zNear) / zRange;

	matrix.m[3][0] = 0.0f;
	matrix.m[3][1] = 0.0f;
	matrix.m[3][2] = 1.0f;
	matrix.m[3][3] = 0.0f;

	return matrix;
}


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