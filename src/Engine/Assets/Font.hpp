#pragma once

#include <map>
#include <string>

#include <GL/glew.h>
#include <Graphics/Color.hpp>
#include <Graphics/GL/GLTexture.hpp>
#include <Graphics/GL/GLSLProgram.hpp>
#include <Math/Vector2.hpp>

struct Glyph {
	uint32 width, height;
	int32 bearingX, bearingY;
	uint32 advance;
	Vector2 uvs[4];
};

struct Font {
	GLuint textureID;
	uint32 bitmapWidth;
	uint32 bitmapHeight;
	Glyph  glyphs[95];

	void GetSize(const std::string& string, uint16* width, uint16* height);
};
