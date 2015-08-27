#pragma once

#include <ECS/Component.hpp>
#include <Math/Vector2.hpp>

struct Transform2D {
	Vector2 position;
	Vector2 size;
	Vector2 rotation;
};
