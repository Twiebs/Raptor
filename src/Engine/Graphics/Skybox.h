#pragma once

#include <vector>

#include <GL/glew.h>

#include <Graphics/Mesh.h>
#include <Graphics/GL/GLSLProgram.h>
#include <Graphics/Camera.h>

class Skybox {
public:
	GLuint vertexArrayID;

	Skybox(std::vector<const char*> faces);
	~Skybox();

	void Draw(float deltaTime);

private:
	GLuint vertexBufferID;
};
