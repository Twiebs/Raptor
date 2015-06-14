#pragma once

#include<ECS\Component.hpp>
#include<Graphics\GL\GLTexture.h>

struct SpriteComponent : public Component {
	float x, y;
	float width, height;
	Color color;
	GLuint textureID;
};