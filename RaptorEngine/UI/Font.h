#pragma once

#include<map>
#include<string>

#include<Graphics\GL\GLTexture.h>
#include<GL\glew.h>
#include<Graphics\GL\GLSLProgram.h>
#include<Graphics\Color.h>
#include<Math\Vector2.hpp>

#include<ft2build.h>
#include FT_FREETYPE_H

//TODO: Add a defualt font fallback for when fonts fail to load...

struct Glyph {
	uint32 width, height;
	int32 berringX, bearingY;
	uint32 advance;
	Vector2 uvs[4];
};

class Font {
public:
	GLuint textureID;
	uint32 bitmapWidth;
	uint32 bitmapHeight;
	std::vector<Glyph> glyphs;

	Font();
	~Font();

	void GetSize(std::string text, float* width, float* height);

private:
};

