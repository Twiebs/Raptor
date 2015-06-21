#include "Matrix4.hpp"
#include <Math/MathUtils.hpp>

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

Matrix4 Matrix4::Ortho(float left, float right, float bottom, float top, float near, float far, float up) {
	float x, y, z;
	float tx, ty, tz;

	x = 2.0f / (right - left);
	y = 2.0f / (top - bottom);
	z = -2.0f / (far - near);
	tx = -(right + left) / (right - left);
	ty = -(top + bottom) / (top - bottom);
	tz = -(far + near) / (far - near);

	y *= up;
	ty *= up;

	Matrix4 r;
	r[0][0] = x;	r[0][1] = 0.0f; r[0][2] = 0.0f; r[0][3] = tx;
	r[1][0] = 0.0f; r[1][1] = y;	r[1][2] = 0.0f; r[1][3] = ty;
	r[2][0] = 0.0f; r[2][1] = 0.0f; r[2][2] = z;	r[2][3] = tz;
	r[3][0] = 0.0f; r[3][1] = 0.0f; r[3][2] = 0.0f; r[3][3] = 1.0f;
	return r;
}
