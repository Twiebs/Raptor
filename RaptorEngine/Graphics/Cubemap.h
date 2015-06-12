#pragma once

#include<vector>
#include<Graphics\Image.h>
#include<GL\glew.h>
#include<Graphics\GL\GLTexture.h>
#include<Core\ResourceManager.h>

class Cubemap{
public:
	GLuint id;
	Cubemap(std::vector<ImageData*>& images);
	Cubemap(std::vector<const char*> faces);
	~Cubemap();
private:

	std::vector<ImageData*> images;
};

