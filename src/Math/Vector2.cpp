#include "Vector2.hpp"

F32 Vector2::Magnitude() {
	return sqrt(x*x + y*y);
}

void Vector2::Normalize() {
	F32 mag = Magnitude();
	if (mag == 0) return;
	x /= mag;
	y /= mag;
}
