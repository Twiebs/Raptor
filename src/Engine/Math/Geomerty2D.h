#pragma once

struct Circle {
    Vector2 center;
    F32 radius;
};

inline bool IsPointInCircle(F32 x, F32 y, Circle* circle);
inline bool IsPointInCircle(F32 x, F32 y, F32 centerX, F32 centerY, F32 radius);


inline bool IsPointInCircle(F32 x, F32 y, Circle* circle) {
    auto result = ((x-circle->center.x)*(x-circle->center.x))
                  +((y-circle->center.y)*(y-circle->center.y))
                  < (circle->radius*circle->radius);
    return result;
}
inline bool IsPointInCircle(F32 x, F32 y, F32 centerX, F32 centerY, F32 radius) {
    auto result = ((x-centerX)*(x-centerX))
                  +((y-centerY)*(y-centerY))
                  < (radius*radius);
    return result;
}
