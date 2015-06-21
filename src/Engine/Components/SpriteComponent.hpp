#pragma once

#include <Core/Component.hpp>
#include <Graphics/Color.hpp>
#include <Graphics/GL/GLTexture.hpp>

struct SpriteComponent : public Component {
	float x, y;
	float width, height;
	Color color;
	GLuint textureID;
};
