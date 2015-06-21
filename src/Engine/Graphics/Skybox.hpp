#pragma once

#include <vector>

#include <GL/glew.h>

#include <Graphics/Mesh.hpp>
#include <Graphics/GL/GLSLProgram.hpp>
#include <Graphics/Camera.hpp>

class Skybox {
public:
	GLuint vertexArrayID;

	Skybox(std::vector<const char*> faces);
	~Skybox();

	void Draw(float deltaTime);

private:
	GLuint vertexBufferID;
};
