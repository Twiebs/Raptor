#pragma once

#include <Graphics/GLSLProgram.hpp>

void CreateSkyboxVertices (GLuint* vao, GLuint* vbo);
void DrawSkybox (GLuint vertexArray, GLuint cubemapID, const Shader& shader);