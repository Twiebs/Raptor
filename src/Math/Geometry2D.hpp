#pragma once

#include <Core/types.h>
#include <Math/Vector2.hpp>

struct Circle {
    Vector2 center;
    float radius;
};

struct Rectangle {
	union {
		V2 position;
		struct { float x, y; };
	};

	union {
		V2 size;
		struct { float w, h; };
	};

	Rectangle(float x, float y, float w, float h)
		: x(x), y(y), w(w), h(h) { }

	//Rectangle(const Rectangle& r) { 
	//	this->position = r.position; 
	//	this->size = r.size;
	//}
};

inline Vector2 GetCenter(const Rectangle& rectangle);
inline Vector2 GetCenter(const Rectangle& r) {
	return Vector2((r.x + r.w)*0.5f, (r.y + r.h)*0.5f);
}

inline bool IsPointInRect(float x, float y, const Rectangle& rect);
inline bool IsPointInRect(float x, float y, const float rectX, const float rectY, const float rectWidth, const float rectHeight);
inline bool IsPointInCircle(float x, float y, const Circle& circle);
inline bool IsPointInCircle(float x, float y, float centerX, float centerY, float radius);

inline bool IsPointInRect(float x, float y, const float rectX, const float rectY, const float rectWidth, const float rectHeight) {
	auto result = (x >= rectX && x <= (rectX + rectWidth) && y >= rectY && y <= (rectY + rectHeight));
	return result;
}

inline bool IsPointInRect(float x, float y, const Rectangle& rect) {
	auto result = (x >= rect.x && x <= (rect.x + rect.w) && y >= rect.y && y <= (rect.y + rect.h));
	return result;
}

inline bool IsPointInCircle(float x, float y, const Circle& circle) {
    auto result = ((x-circle.center.x)*(x-circle.center.x))
                  +((y-circle.center.y)*(y-circle.center.y))
                  < (circle.radius*circle.radius);
    return result;
}

inline bool IsPointInCircle(F32 x, F32 y, F32 centerX, F32 centerY, F32 radius) {
    auto result = ((x-centerX)*(x-centerX))
                  +((y-centerY)*(y-centerY))
                  < (radius*radius);
    return result;
}
