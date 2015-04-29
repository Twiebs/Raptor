#pragma once

#include<map>
#include<string>

#include<GL\glew.h>

#include<Graphics\Shader.h>

#include<UI\Color.h>

#include<ft2build.h>
#include FT_FREETYPE_H

//XXX Rename character to glyph
//XXX Add a defualt font fallback for when fonts fail to load...


struct Character {
	int width, height;
	int bearingX, bearingY;
	GLuint textureID;
	GLuint advance;
};

class Font {
public:
	GLuint vertexArrayID;
	GLuint vertexBufferID;

	std::map<GLchar, Character> characters;

	Font(const char* filename, int fontSize);
	~Font();

	void RenderText(ShaderProgram* shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, Color* color);

private:

	void InitGLBuffers();
};

