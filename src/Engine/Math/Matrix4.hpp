#pragma once

#include <assert.h>
#include <Math/Vector3.hpp>
#include <Math/MathUtils.hpp>

class Matrix4 {
public:
	float32 m[4][4];

	inline const float* operator[](int i) const {
		assert(i >= 0);
		assert(i < 4);
		return (const float*)&m[i];
	}

	inline float* operator[](int i) {
		assert(i >= 0);
		assert(i < 4);
		return (float*)&m[i];
	}

	inline operator float* () {
		return (float*)&m[0][0];
	}

	inline Matrix4 operator*(const Matrix4 & right) const{
		Matrix4 ret;

		for (unsigned int i = 0; i < 4; i++) {
			for (unsigned int j = 0; j < 4; j++) {
				ret.m[i][j] =
					m[i][0] * right[0][j] +
					m[i][1] * right[1][j] +
					m[i][2] * right[2][j] +
					m[i][3] * right[3][j];
			}
		}
		return ret;
	}

	//Returns an identity matrix
	inline static Matrix4 Identity() {
		static Matrix4 r = {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		}; return r;
	}

	//Returns a matrix with all zeros
	inline static Matrix4 Zero() {
			static Matrix4 r = {
				0.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 0.0f
			}; return r;
		}




	//Creates an Orthographic Projection matrix
	inline static Matrix4 Ortho(float32 left, float32 right, float32 bottom, float32 top, float32 near, float32 far, float32 up) {
		float32 x = 2.0f / (right - left);
		float32 y = 2.0f / (top - bottom);
		float32 z = -2.0f / (far - near);
		float32 tx = -(right + left) / (right - left);
		float32 ty = -(top + bottom) / (top - bottom);
		float32 tz = -(far - near) / (far - near);

		y *= up;
		ty *= up;

		Matrix4 r {
			x, 		0.0f,	0.0f, 	tx,
			0.0f, 	y, 		0.0f, 	ty,
			0.0f, 	0.0f, 	z, 		tz,
			0.0f, 	0.0f, 	0.0f, 	1.0f
		}; return r;
	}

	inline static Matrix4 Ortho(float32 left, float32 right, float32 bottom, float32 top, float32 up) {
		float32 x = 2.0f / (right - left);
		float32 y = 2.0f / (top - bottom);
		float32 tx = -(right + left) / (right - left);
		float32 ty = -(top + bottom) / (top - bottom);

		y *= up;
		ty *= up;

		Matrix4 r;
		r.m[0][0] = x;    r.m[1][0] = 0.0f; r.m[2][0] = 0.0f; r.m[3][0] = tx;
		r.m[0][1] = 0.0f; r.m[1][1] = y;    r.m[2][1] = 0.0f; r.m[3][1] = ty;
		r.m[0][2] = 0.0f; r.m[1][2] = 0.0f; r.m[2][2] = -1.0f; r.m[3][2] = 1.0f;
		r.m[0][3] = 0.0f; r.m[1][3] = 0.0f; r.m[2][3] = 0.0f; r.m[3][3] = 1.0f;
		return r;
	}

	//Shorthand Orthographic projection
	inline static Matrix4 Ortho(float32 width, float32 height) {
		float32 x = 2.0f / width;
		float32 y = 2.0f / height;
		float32 z = -2.0f / 2.0f;
		float32 tx = -width / width;
		float32 ty = -height / height;
		float32 tz = -2.0f/2.0f;


		Matrix4 r {
			x, 		0.0f,	0.0f, 	tx,
			0.0f, 	y, 		0.0f, 	ty,
			0.0f, 	0.0f, 	z, 		tz,
			0.0f, 	0.0f, 	0.0f, 	1.0f
		}; return r;
	}

	//Creates a translation transform matrix
	inline static Matrix4 Translate(float x, float y, float z) {
		Matrix4 r;
		r[0][0] = 1.0f; r[0][1] = 0.0f; r[0][2] = 0.0f; r[0][3] = x;
		r[1][0] = 0.0f; r[1][1] = 1.0f; r[1][2] = 0.0f; r[1][3] = y;
		r[2][0] = 0.0f; r[2][1] = 0.0f; r[2][2] = 1.0f; r[2][3] = z;
		r[3][0] = 0.0f; r[3][1] = 0.0f; r[3][2] = 0.0f; r[3][3] = 1.0f;
		return r;
	}

	//Creates a scale transform matrix
	inline static Matrix4 Scale(float sclX, float sclY, float sclZ) {
		Matrix4 r;
		r[0][0] = sclX;	r[0][1] = 0.0f;	r[0][2] = 0.0f;	r[0][3] = 0.0f;
		r[1][0] = 0.0f;	r[1][1] = sclY;	r[1][2] = 0.0f;	r[1][3] = 0.0f;
		r[2][0] = 0.0f;	r[2][1] = 0.0f;	r[2][2] = sclZ;	r[2][3] = 0.0f;
		r[3][0] = 0.0f;	r[3][1] = 0.0f;	r[3][2] = 0.0f;	r[3][3] = 1.0f;
		return r;
	}

	//Creates a rotation transform matrix
	inline static Matrix4 Rotate(float deltaX, float deltaY, float deltaZ) {
		Matrix4 rx, ry, rz;

		const float x = MathUtils::Radians(deltaX);
		const float y = MathUtils::Radians(deltaY);
		const float z = MathUtils::Radians(deltaZ);

		rx[0][0] = 1.0f; rx[0][1] = 0.0f;	rx[0][2] = 0.0f;	rx[0][3] = 0.0f;
		rx[1][0] = 0.0f; rx[1][1] = cos(x); rx[1][2] = -sin(x); rx[1][3] = 0.0f;
		rx[2][0] = 0.0f; rx[2][1] = sin(x); rx[2][2] = cos(x);	rx[2][3] = 0.0f;
		rx[3][0] = 0.0f; rx[3][1] = 0.0f;	rx[3][2] = 0.0f;	rx[3][3] = 1.0f;

		ry[0][0] = cos(y);	ry[0][1] = 0.0f; ry[0][2] = -sin(y);	ry[0][3] = 0.0f;
		ry[1][0] = 0.0f;	ry[1][1] = 1.0f; ry[1][2] = 0.0f;		ry[1][3] = 0.0f;
		ry[2][0] = sin(y);	ry[2][1] = 0.0f; ry[2][2] = cos(y);		ry[2][3] = 0.0f;
		ry[3][0] = 0.0f;	ry[3][1] = 0.0f; ry[3][2] = 0.0f;		ry[3][3] = 1.0f;

		rz[0][0] = cos(z);	rz[0][1] = -sin(z);	rz[0][2] = 0.0f; rz[0][3] = 0.0f;
		rz[1][0] = sin(z);	rz[1][1] = cos(z);	rz[1][2] = 0.0f; rz[1][3] = 0.0f;
		rz[2][0] = 0.0f;	rz[2][1] = 0.0f;	rz[2][2] = 1.0f; rz[2][3] = 0.0f;
		rz[3][0] = 0.0f;	rz[3][1] = 0.0f;	rz[3][2] = 0.0f; rz[3][3] = 1.0f;

		return rz * ry * rx;
	}
};
