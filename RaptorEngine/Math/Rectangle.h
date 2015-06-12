#pragma once

#include<Math\Vector2.hpp>

class Rectangle {
public:
	float x, y;
	float width, height;

	Rectangle();
	Rectangle(float x, float y, float width, float height);
	~Rectangle();


	inline float Left() { return x; }
	inline float Right() { return x + width; }
	inline float Bottom() { return y; }
	inline float Top() { return y + height; }
	inline Vector2 BottomLeft()  { return Vector2(x, y); }
	inline Vector2 BottomRight() { return Vector2(x + width, y); }
	inline Vector2 TopLeft()	 { return Vector2(x, y + height); }
	inline Vector2 TopRight()	 { return Vector2(x + width, y + height); }

	bool Contains(float x, float y);
	bool Contains(Vector2 point);
	Vector2 Center();

};

