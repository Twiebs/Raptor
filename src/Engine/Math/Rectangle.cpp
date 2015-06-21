#include "Rectangle.hpp"


Rectangle::Rectangle() {
}


Rectangle::~Rectangle() {
}

Rectangle::Rectangle(float x, float y, float width, float height) {
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
}

bool Rectangle::Contains(Vector2 point) {
	return Contains(point.x, point.y);
}

bool Rectangle::Contains(float x, float y) {
	return	(x >= this->x) && 
			(x <= (this->x + this->width)) &&
			(y >= this->y) && 
			(y <= (this->y + this->height));
}


Vector2 Rectangle::Center() {
	return Vector2(this->x + (this->width * 0.5f), this->y + (this->height * 0.5f));
}

