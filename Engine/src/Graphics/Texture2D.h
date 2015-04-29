#pragma once

#include<string>

#include<GL\glew.h>

class Texture2D {
public:
	Texture2D(char* filename);

	bool Load();
	void Bind(GLenum textureUnit);

private:
	char* filename;

	GLenum textureTarget;
	GLuint textureObject;
};

