#pragma once

#include <Core/Common.hpp>
#include <Math/Vector2.hpp>

struct Circle {
    Vector2 center;
    float radius;
};

struct Rectangle {
	float x, y;
	float width, height;
};

inline bool IsPointInRect(float x, float y, const Rectangle& rect);
inline bool IsPointInRect(float x, float y, const float rectX, const float rectY, const float rectWidth, const float rectHeight);
inline bool IsPointInCircle(float x, float y, const Circle& circle);
inline bool IsPointInCircle(float x, float y, float centerX, float centerY, float radius);

inline bool IsPointInRect(float x, float y, const float rectX, const float rectY, const float rectWidth, const float rectHeight) {
	auto result = (x >= rectX && x <= (rectX + rectWidth) && y >= rectY && y <= (rectY + rectHeight));
	return result;
}

inline bool IsPointInRect(float x, float y, const Rectangle& rect) {
	auto result = (x >= rect.x && x <= (rect.x + rect.width) && y >= rect.y && y <= (rect.y + rect.height));
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
