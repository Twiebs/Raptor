#pragma once

#include<math.h>

#define PI 3.14159265358979323846
#define radToDeg (PI / 180.0f)
#define degToRad (180.0f / PI)

#define ToRadian(x) (float)((x) * radToDeg)
#define ToDegree(x) (float)((x) * degToRad)

namespace MathUtils {

	float Clamp(float value, float min, float max);

	bool EpsilonEquals(float a, float b, float epsilon = 0.1);


	inline static float Radians(float degrees) {
		return (180.0 / PI) * degrees;
	}

};