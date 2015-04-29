#include "Color.h"


Color::Color() : r(1.0f), g(1.0f), b(1.0f) { }

Color::Color(float r, float g, float b) {
	this->r = r;
	this->g = g;
	this->b = b;
}


Color::~Color() {
}
