#pragma once

#include <string>

#include <Core/Common.hpp>
#include <GL/glew.h>

enum class TextureType : uint8 {
	Diffuse,
	Specular,
	Cubemap,
};

struct GLTexture {
	GLuint id;
	TextureType type;
	GLenum target;
};
