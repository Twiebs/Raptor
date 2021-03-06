#pragma once

#include <iostream>
#include <cmath>


struct Vector4 {
	float x, y, z, w;
	Vector4(float x, float y, float z, float w);
};

struct Vector3 {
	float x, y, z;

	Vector3();
	Vector3(float x, float y, float z);
	//Initalizes a new vector with each component being the given value.
	Vector3(float value);


	void Set(float x, float y, float z);
	void Set(const Vector3* vector);

    inline float magnitude();

	float Distance(Vector3& other);

	Vector3& Normalize();
	Vector3 Cross(const Vector3& vector) const;
	inline float Dot(const Vector3& vector) const;

	friend std::ostream& operator<< (std::ostream& os, const Vector3& vector) {
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

	Vector3& operator*=(float r) {
		x *= r;
		y *= r;
		z *= r;
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

//	friend Vector3 operator-(const Vector3& left, const Vector3& right) {
//		Vector3 vector(
//			left.x - right.x,
//			left.y - right.y,
//			left.z - right.z
//			);
//		return vector;
//	}


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

};

typedef Vector3 V3;
typedef Vector4 V4;

float Vector3::Dot(const Vector3& v) const {
	return (x*v.x) + (y*v.y) + (z*v.z);
}

inline float Vector3::magnitude() {
    return std::sqrt((x * x) + (y * y) + (z * z));
}

//inline Vector3&& operator- (const Vector3& a, const Vector3& b) {
//    const float x = a.x - b.x;
//    const float y = a.y - b.y;
//    const float z = a.z - b.z;
//    return std::move<Vector3>(Vector3(x, y, z));
//}

//inline Vector3 operator- (const Vector3& left, const Vector3& right) {
//	Vector3 vector(
//			left.x - right.x,
//			left.y - right.y,
//			left.z - right.z );
//	return vector;
//}

inline Vector3 operator- (const Vector3& a, const Vector3& b) {
	Vector3 result = {};
	result.x = a.x - b.x;
	result.y = a.y - b.y;
	result.z = a.z - b.z;
	return result;
}

inline V3 operator-(const V3& a) {
	return V3(-a.x, -a.y, -a.z);
}

inline V3 operator*(const V3& a, float s) {
	return V3(a.x * s, a.y * s, a.z * s);
}


inline Vector3 Cross (const Vector3& a, const Vector3& b);
inline float Dot (const Vector3& a, const Vector3& b);
inline float Magnitude (const Vector3& a);
inline float Distance (const Vector3& a, const Vector3& b);
inline float DistanceSquared (const Vector3& a, const Vector3& b);
inline V3 Normalize(const V3& a);

inline Vector3 Cross (const Vector3& a, const Vector3& b) {
    const float x = (a.y * b.z) - (a.z * b.y);
    const float y = (a.z * b.x) - (a.x * b.z);
    const float z = (a.x * b.y) - (a.y * b.x);
    return Vector3(x, y, z);
}

inline float Dot (const Vector3& a, const Vector3& b) {
    float result = (a.x * b.x) + (a.y * b.y) + (a.z + b.z);
    return result;
}

inline float Magnitude (const Vector3& a) {
    float result = sqrtf((a.x * a.x) + (a.y * a.y) + (a.z * a.z));
    return result;
}

inline float Distance (const Vector3& a, const Vector3& b) {
    auto displacement = b - a;
    auto result = Magnitude(displacement);
    return result;
}

inline float DistanceSquared (const Vector3& a, const Vector3& b) {
    auto c = b - a;
    auto result = ((c.x*c.x) + (c.y*c.y) + (c.z*c.z));
    return result;
}

inline V3 Normalize(const V3& v) {
	const float magnitude = Magnitude(v);
	V3 result = { };
	result.x = v.x / magnitude;
	result.y = v.y / magnitude;
	result.x = v.z / magnitude;
	return result;
}

