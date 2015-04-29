#pragma once

#include<vector>

#include<GL\glew.h>

#include<Graphics\Mesh.h>
#include<Graphics\Shader.h>
#include<Graphics\Texture.h>
#include<Graphics\Camera.h>

class Skybox {
public:
	GLuint vertexArrayID;
	CubemapTexture* cubemap;

	Skybox(std::vector<const char*> faces);
	~Skybox();

	void Draw(float deltaTime);

private:
	GLuint vertexBufferID;
};

