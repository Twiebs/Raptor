#pragma once

#include <assert.h>
#include <Math/Vector3.hpp>
#include <Math/Math.hpp>

class Matrix4 {
public:
	float m[4][4];

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
		Matrix4 result = Matrix4::Zero();
		for (U32 r = 0; r < 4; r++) {
			for (U32 c = 0; c < 4; c++) {
				for (U32 i = 0; i < 4; i++) {
					result.m[c][r] += m[i][r] * right[c][i];
				}
			}
		}
		return result;
	}

	//Returns an identity matrix
	inline static Matrix4 Identity();
	//Returns a matrix with all zeros
	inline static Matrix4 Zero();

	//Creates an Orthographic Projection matrix
	inline static Matrix4 Ortho(float left, float right, float bottom, float top, float near, float far, float up) {
		float x = 2.0f / (right - left);
		float y = 2.0f / (top - bottom);
		float z = -2.0f / (far - near);
		float tx = -(right + left) / (right - left);
		float ty = -(top + bottom) / (top - bottom);
		float tz = -(far - near) / (far - near);

		y *= up;
		ty *= up;

		Matrix4 r;
		r[0][0] = x;		r[1][0] = 0.0f;		r[2][0] = 0.0f;		r[3][0] = tx;
		r[0][1] = 0.0f;		r[1][1] = y;		r[2][1] = 0.0f;		r[3][1] = ty;
		r[0][2] = 0.0f;		r[1][2] = 0.0f;		r[2][2] = z;		r[3][2] = tz;
		r[0][3] = 0.0f;		r[1][3] = 0.0f;		r[2][3] = 0.0f;		r[3][3] = 1.0f;
		return r;
	}


	inline static Matrix4 Ortho(float left, float right, float bottom, float top, float up) {
		float x = 2.0f / (right - left);
		float y = 2.0f / (top - bottom);
		float tx = -(right + left) / (right - left);
		float ty = -(top + bottom) / (top - bottom);

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
	inline static Matrix4 Ortho(float width, float height) {
		float x = 2.0f / width;
		float y = 2.0f / height;
		float z = -2.0f / 2.0f;
		float tx = -width / width;
		float ty = -height / height;
		float tz = -2.0f/2.0f;


		Matrix4 r {
			x, 		0.0f,	0.0f, 	tx,
			0.0f, 	y, 		0.0f, 	ty,
			0.0f, 	0.0f, 	z, 		tz,
			0.0f, 	0.0f, 	0.0f, 	1.0f
		}; return r;
	}

	inline static Matrix4 Perspective(F32 fieldOfView, F32 aspectRatio, F32 nearClip, F32 farClip);

	inline static Matrix4 Translate(const Vector3& translation);
	inline static Matrix4 Translate(float dx, float dy, float dz);

    inline static Matrix4 Rotate(const Vector3& rotation);

	inline static Matrix4 Scale(const Vector3& scale);
	inline static Matrix4 Scale(float sclX, float sclY, float sclZ);

    inline static Matrix4 Transform(const Vector3& translation, const Vector3& rotation, const Vector3& scale);

	inline static Matrix4 LookAt(Vector3 position, Vector3 target, Vector3 up);
};

Matrix4 Matrix4::Identity() {
	static Matrix4 r = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	}; return r;
}

Matrix4 Matrix4::Zero() {
	static Matrix4 r = {
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f
	}; return r;
}

Matrix4 Matrix4::Perspective(F32 fieldOfView, F32 aspectRatio, F32 zNear, F32 zFar) {
	assert(aspectRatio != 0.0);
	assert(zNear != zFar);
	
	const F32 tanHalfFOV = tan(Radians(fieldOfView) * 0.5f);

	Matrix4 r = Matrix4::Zero();
	r[0][0] = 1.0f / (aspectRatio * tanHalfFOV);
	r[1][1] = 1.0f / (tanHalfFOV);
	r[2][2] = -(zFar + zNear) / (zFar - zNear);
	r[2][3] = -1.0f;
	r[3][2] = -(2 * zFar * zNear) / (zFar - zNear);
	return r;

#if 0
	const F32 clipRange = farClip - nearClip;
	const F32 tanHalfFOV = tan(fieldOfView * 0.5f);
	Matrix4 r;

	r[0][0] = 1.0f / (tanHalfFOV * aspectRatio);
	r[0][1] = 0.0f;
	r[0][2] = 0.0f;
	r[0][3] = 0.0f;

	r[1][0] = 0.0f;
	r[1][1] = 1.0f / tanHalfFOV;
	r[1][2] = 0.0f;
	r[1][3] = 0.0f;

	r[2][0] = 0.0f;
	r[2][1] = 0.0f;
	r[2][2] = (-nearClip - farClip) / -clipRange;
	r[2][3] = 2.0f * farClip * nearClip / -clipRange;

	r[3][0] = 0.0f;
	r[3][1] = 0.0f;
	r[3][2] = 1.0f;
	r[3][3] = 0.0f;
	return r;
#endif
}

inline Matrix4 Matrix4::Translate(const Vector3& translation) {
	Matrix4 r;
	r[0][0] = 1.0f; r[1][0] = 0.0f; r[2][0] = 0.0f; r[3][0] = translation.x;
	r[0][1] = 0.0f; r[1][1] = 1.0f; r[2][1] = 0.0f; r[3][1] = translation.y;
	r[0][2] = 0.0f; r[1][2] = 0.0f; r[2][2] = 1.0f; r[3][2] = translation.z;
	r[0][3] = 0.0f; r[1][3] = 0.0f; r[2][3] = 0.0f; r[3][3] = 1.0f;
	return r;
}

inline Matrix4 Matrix4::Translate(float dx, float dy, float dz) {
	Matrix4 r;
	r[0][0] = 1.0f; r[1][0] = 0.0f; r[2][0] = 0.0f; r[3][0] = dx;
	r[0][1] = 0.0f; r[1][1] = 1.0f; r[2][1] = 0.0f; r[3][1] = dy;
	r[0][2] = 0.0f; r[1][2] = 0.0f; r[2][2] = 1.0f; r[3][2] = dz;
	r[0][3] = 0.0f; r[1][3] = 0.0f; r[2][3] = 0.0f; r[3][3] = 1.0f;
	return r;
}

inline Matrix4 Matrix4::Rotate(const Vector3& rotation) {
    const float x = Radians(rotation.x);
    const float y = Radians(rotation.y);
    const float z = Radians(rotation.z);

    Matrix4 rx, ry, rz;
    rx[0][0] = 1.0f; rx[1][0] = 0.0f;	rx[2][0] = 0.0f;	rx[3][0] = 0.0f;
    rx[0][1] = 0.0f; rx[1][1] = cos(x); rx[2][1] = -sin(x); rx[3][1] = 0.0f;
    rx[0][2] = 0.0f; rx[1][2] = sin(x); rx[2][2] = cos(x);	rx[3][2] = 0.0f;
    rx[0][3] = 0.0f; rx[1][3] = 0.0f;	rx[2][3] = 0.0f;	rx[3][3] = 1.0f;

    ry[0][0] = cos(y);	ry[1][0] = 0.0f; ry[2][0] = -sin(y);	ry[3][0] = 0.0f;
    ry[0][1] = 0.0f;	ry[1][1] = 1.0f; ry[2][1] = 0.0f;		ry[3][1] = 0.0f;
    ry[0][2] = sin(y);	ry[1][2] = 0.0f; ry[2][2] = cos(y);		ry[3][2] = 0.0f;
    ry[0][3] = 0.0f;	ry[1][3] = 0.0f; ry[2][3] = 0.0f;		ry[3][3] = 1.0f;

    rz[0][0] = cos(z);	rz[1][0] = -sin(z);	rz[2][0] = 0.0f; rz[3][0] = 0.0f;
    rz[0][1] = sin(z);	rz[1][1] = cos(z);	rz[2][1] = 0.0f; rz[3][1] = 0.0f;
    rz[0][2] = 0.0f;	rz[1][2] = 0.0f;	rz[2][2] = 1.0f; rz[3][2] = 0.0f;
    rz[0][3] = 0.0f;	rz[1][3] = 0.0f;	rz[2][3] = 0.0f; rz[3][3] = 1.0f;
    return rz * ry * rx;
}

inline Matrix4 Matrix4::Scale(const Vector3& scale) {
	Matrix4 r;
	r[0][0] = scale.x;	r[1][0] = 0.0f;		r[2][0] = 0.0f;		r[3][0] = 0.0f;
	r[0][1] = 0.0f;		r[1][1] = scale.y;	r[2][1] = 0.0f;		r[3][1] = 0.0f;
	r[0][2] = 0.0f;		r[1][2] = 0.0f;		r[2][2] = scale.z;	r[3][2] = 0.0f;
	r[0][3] = 0.0f;		r[1][3] = 0.0f;		r[2][3] = 0.0f;		r[3][3] = 1.0f;
	return r;
}

inline Matrix4 Matrix4::Scale(float sclX, float sclY, float sclZ) {
	Matrix4 r;
	r[0][0] = sclX;		r[1][0] = 0.0f;		r[2][0] = 0.0f;		r[3][0] = 0.0f;
	r[0][1] = 0.0f;		r[1][1] = sclY;		r[2][1] = 0.0f;		r[3][1] = 0.0f;
	r[0][2] = 0.0f;		r[1][2] = 0.0f;		r[2][2] = sclZ;		r[3][2] = 0.0f;
	r[0][3] = 0.0f;		r[1][3] = 0.0f;		r[2][3] = 0.0f;		r[3][3] = 1.0f;
	return r;
}

inline Matrix4 Matrix4::Transform(const Vector3& translation, const Vector3& rotation, const Vector3& scale) {
    return Matrix4::Translate(translation) * Matrix4::Rotate(rotation) * Matrix4::Scale(scale);
}

Matrix4 Matrix4::LookAt(Vector3 position, Vector3 target, Vector3 up) {
	const Vector3 f = (target - position).Normalize();
	const Vector3 s = (f.Cross(up).Normalize());
	const Vector3 u = s.Cross(f);

	Matrix4 r = Matrix4::Identity();
	r[0][0] = s.x;
	r[1][0] = s.y;
	r[2][0] = s.z;
	r[0][1] = u.x;
	r[1][1] = u.y;
	r[2][1] = u.z;
	r[0][2] = -f.x;
	r[1][2] = -f.y;
	r[2][2] = -f.z;
	r[3][0] = -s.Dot(position);
	r[3][1] = -u.Dot(position);
	r[3][2] = f.Dot(position);
	return r;
}