#include "Vector2.hpp"

float32 Vector2::Length() {
	return sqrt(x*x + y*y);
}

void Vector2::Normalize() {
	float32 length = Length();
	if(length == 0) {
		return;
	}
	x /= length;
	y /= length;
}
