#pragma once

#include "Vector3.h"

struct PerspectiveProjectionInfo {
	float fov;
	float width;
	float height;
	float zNear;
	float zFar;
};

class Matrix4 {
public:
	float m[4][4];

	void initScaleTransform(float scaleX, float scaleY, float scaleZ);
	void initRotateTransform(float rotateX, float rotateY, float rotateZ);

	void initTranslationTransform(Vector3 position);
	void initTranslationTransform(float x, float y, float z);

	void initPerspectiveTransform(const PerspectiveProjectionInfo& perspectiveProjectionInfo);

	inline Matrix4 operator*(const Matrix4 & right) const{
		Matrix4 ret;

		for (unsigned int i = 0; i < 4; i++) {
			for (unsigned int j = 0; j < 4; j++) {
				ret.m[i][j] = 
					m[i][0] * right.m[0][j] +
					m[i][1] * right.m[1][j] +
					m[i][2] * right.m[2][j] +
					m[i][3] * right.m[3][j];
			}
		}
		return ret;
	}

};

