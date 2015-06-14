
#include <Math\Vector2.hpp>
#include <ECS\Component.hpp>

struct Transform2D : public Component {
	Vector2 position;
	Vector2 scale;
	Vector2 rotation;
};