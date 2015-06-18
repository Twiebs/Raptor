#pragma once
#include <Math\Vector2.hpp>
#include <Core\Component.hpp>

struct Transform2D : public Component {
	Vector2 position;
	Vector2 size;
	Vector2 rotation;
};