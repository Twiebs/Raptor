#pragma once

#include<Math\Matrix.h>

#define PI 3.14159265358979323846

#define radToDeg (PI / 180.0f)
#define degToRad (180.0f / PI)

#define ToRadian(x) (float)((x) * radToDeg)
#define ToDegree(x) (float)((x) * degToRad)

namespace MathUtils {

	Matrix4 PerspectiveProjection(const float fov, const float ar, const float zNear, const float zFar);

	float Clamp(float value, float min, float max);

	bool EpsilonEquals(float a, float b, float epsilon = FLT_EPSILON);

};