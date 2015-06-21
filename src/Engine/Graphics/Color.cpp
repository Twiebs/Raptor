#include "Color.hpp"


Color::Color() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) { }

Color::Color(float r, float g, float b, float a) {
	this->r = r;
	this->g = g;
	this->b = b;
	this->a = a;
}


Color::~Color() {
}
