#pragma once

#include <ECS/Component.hpp>
#include <Graphics/Color.hpp>
#include <Graphics/GLTexture.hpp>


//Components should not hold any data...
//They simply should just point to the data that they are refrencing...
struct SpriteComponent : public Component {
	float x, y;
	float width, height;
	Color color;
	GLuint textureID;
};
