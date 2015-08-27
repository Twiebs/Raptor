
#include "Matrix4.hpp"

//void Matrix4::initPerspectiveTransform(const PerspectiveProjectionInfo & perspectiveProjectionInfo) {
//	float ar = perspectiveProjectionInfo.width / perspectiveProjectionInfo.height;
//
//	const float zNear = perspectiveProjectionInfo.zNear;
//	const float zFar = perspectiveProjectionInfo.zFar;
//	const float zRange = zNear - zFar;
//	const float tanHalfFOV = tanf(ToRadian(perspectiveProjectionInfo.fov / 2.0));
//
//	m[0][0] = 1.0f / (tanHalfFOV * ar);
//	m[0][1] = 0.0f;
//	m[0][2] = 0.0f;
//	m[0][3] = 0.0f;
//
//	m[1][0] = 0.0f;
//	m[1][1] = 1.0f / (tanHalfFOV * ar);
//	m[1][2] = 0.0f;
//	m[1][3] = 0.0f;
//
//	m[2][0] = 0.0f;
//	m[2][1] = 0.0f;
//	m[2][2] = (-zNear - zFar) / zRange;
//	m[2][3] = (2.0f * zFar * zNear) / zRange;
//
//	m[3][0] = 0.0f;
//	m[3][1] = 0.0f;
//	m[3][2] = 1.0f;
//	m[3][3] = 0.0f;
//}
