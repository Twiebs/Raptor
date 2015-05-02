#pragma once

#include<vector>
#include<GL\glew.h>
#include<Core\ResourceManager.h>

struct TextureData {
	std::string filename;
	int width, height;
	char* pixels;
};

class Texture {
public:
	GLuint textureID;
	GLenum textureTarget;
	Texture(GLenum textureTarget);
	~Texture();

	void Bind();

	//Static creation stuff
	static TextureData* LoadTexture(const char* filename);

protected:

};

class CubemapTexture : public Texture {
public:
	CubemapTexture(std::vector<const char*> faces);
	~CubemapTexture();
};

