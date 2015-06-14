#pragma once

#include<ECS\Component.hpp>
#include<Math\Vector3.hpp>

struct Transform3D : public Component {
	Vector3 position;
	Vector3 rotation;
	Vector3 scale;
};
