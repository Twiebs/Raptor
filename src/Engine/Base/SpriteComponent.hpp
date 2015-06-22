#pragma once

#include <ECS/Component.hpp>
#include <Graphics/Color.hpp>
#include <Graphics/GLTexture.hpp>

struct SpriteComponent : public Component {
	float x, y;
	float width, height;
	Color color;
	GLuint textureID;
};
