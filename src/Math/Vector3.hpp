#pragma once

#include <iostream>
#include <cmath>

struct Vector4 {
	float x, y, z, w;
	Vector4(float x, float y, float z, float w);
};

class Vector3 {
public:
	float x;
	float y;
	float z;

	Vector3();
	Vector3(float x, float y, float z);
	//Initalizes a new vector with each component being the given value.
	Vector3(float value);
	~Vector3();

	void Set(float x, float y, float z);
	void Set(const Vector3* vector);

    inline float magnitude();

	float Distance(Vector3& other);

	Vector3& Normalize();
	Vector3 Cross(const Vector3& vector) const;
	inline float Dot(const Vector3& vector) const;

	friend std::ostream& operator<<(std::ostream& os, const Vector3& vector) {
		os << "[" << vector.x << ", " << vector.y << ", " << vector.z << "]";
		return os;
	}

	Vector3& operator+=(const Vector3 &right) {
		x += right.x;
		y += right.y;
		z += right.z;
		return *this;
	}

	Vector3& operator-=(const Vector3 &right) {
		x -= right.x;
		y -= right.y;
		z -= right.z;
		return *this;
	}

	friend Vector3 operator+(const Vector3& left, const Vector3& right) {
		Vector3 vector (
			left.x + right.x,
			left.y + right.y,
			left.z + right.z
			);
		return vector;
	}

	friend Vector3 operator-(const Vector3& left, const Vector3& right) {
		Vector3 vector(
			left.x - right.x,
			left.y - right.y,
			left.z - right.z
			);
		return vector;
	}

	friend Vector3 operator*(const Vector3& vector, float scalar) {
		Vector3 returnVector(
			vector.x * scalar,
			vector.y * scalar,
			vector.z * scalar
			);
		return returnVector;
	}

	friend Vector3 operator/(const Vector3& vector, float divisor) {
		Vector3 returnVector(
			vector.x / divisor,
			vector.y / divisor,
			vector.z / divisor
			);
		return returnVector;
	}

	friend Vector3 operator*(const Vector3& vectorA, const Vector3& vectorB) {
		Vector3 returnVector(
			vectorA.x * vectorB.x,
			vectorA.y * vectorB.y,
			vectorA.z * vectorB.z
			);
		return returnVector;
	}

private:

};

float Vector3::Dot(const Vector3& v) const {
	return (x*v.x) + (y*v.y) + (z*v.z);
}

inline float Vector3::magnitude() {
    return std::sqrt((x * x) + (y * y) + (z * z));
}