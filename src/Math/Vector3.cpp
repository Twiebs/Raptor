#include "Vector3.hpp"


Vector4::Vector4(float x, float y, float z, float w)
	: x(x), y(y), z(z), w(w) { }

Vector3::Vector3() {
	this->x = 0;
	this->y = 0;
	this->z = 0;
}

Vector3::Vector3(float x, float y, float z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

Vector3::Vector3(float value) {
	this->x = value;
	this->y = value;
	this->z = value;
}

void Vector3::Set(float x, float y, float z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

void Vector3::Set(const Vector3* vector) {
	this->x = vector->x;
	this->y = vector->y;
	this->z = vector->z;
}

Vector3& Vector3::Normalize() {
	const float length = Length();

	this->x /= length;
	this->y /= length;
	this->z /= length;

	return *this;
}

float Vector3::Length() {
	return std::sqrt((x * x) + (y * y) + (z * z));
}

float Vector3::Distance(Vector3& other) {
	Vector3 temp = *this - other;
	return temp.Length();
}


Vector3 Vector3::Cross(const Vector3& vector) const {
	const float _x = (y * vector.z) - (z * vector.y);
	const float _y = (z * vector.x) - (x * vector.z);
	const float _z = (x * vector.y) - (y * vector.x);

	return Vector3(_x, _y, _z);
}


Vector3::~Vector3() {
}
