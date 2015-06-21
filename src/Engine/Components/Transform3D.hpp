#pragma once

#include <Core/Component.hpp>
#include <Math/Vector3.hpp>

struct Transform3D : public Component {
	Vector3 position;
	Vector3 size;
	Vector3 rotation;
};
