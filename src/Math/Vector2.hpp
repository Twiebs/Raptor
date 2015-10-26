#pragma once

#include <Core/types.h>
#include <Math/Math.hpp>

#include <iostream>

class Vector2 {
public:
	F32 x, y;
	Vector2() { x = 0; y = 0; }
	Vector2(float x, float y) : x(x), y(y) { }
	~Vector2() { }

	friend std::ostream& operator<<(std::ostream& os, const Vector2& vector) {
		os << "[" << vector.x << ", " << vector.y << "]";
		return os;
	}

	Vector2& operator+=(const Vector2& right) {
		x += right.x;
		y += right.y;
		return *this;
	}
	Vector2& operator*=(const Vector2& right) {
		x *= right.x;
		y *= right.y;
		return *this;
	}

	Vector2& operator*=(const float scalar) {
		x *= scalar;
		y *= scalar;
		return *this;
	}

	friend Vector2 operator+(const Vector2& left, const Vector2& right) {
		return Vector2(left.x + right.x, left.y + right.y);
	}
	friend Vector2 operator*(const Vector2& left, const Vector2& right) {
		return Vector2(left.x * right.x, left.y * right.y);
	}
	friend Vector2 operator*(const Vector2& left, const F32 scalar) {
		return Vector2(left.x * scalar, left.y * scalar);
	}
	friend Vector2 operator*(const F32 scalar, const Vector2& left) {
		return Vector2(left.x * scalar, left.y * scalar);
	}
	friend Vector2 operator-(const Vector2& left, const Vector2& right) {
		return Vector2(left.x - right.x, left.y - right.y);
	}

	F32 Magnitude();
	void Normalize();
};
