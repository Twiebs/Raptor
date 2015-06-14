#pragma once

#include<GL\glew.h>
#include<Graphics\Image.h>

class GLTexture {
public:
	GLuint id;
	ImageData data;
	std::string textureType;

	GLTexture(ImageData& data);
	~GLTexture();

};

