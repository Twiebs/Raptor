#pragma once

#include <Core/Component.hpp>
#include <Graphics/Color.h>
#include <Graphics/GL/GLTexture.h>

struct SpriteComponent : public Component {
	float x, y;
	float width, height;
	Color color;
	GLuint textureID;
};
