#pragma once

#include <Core/Common.hpp>
#include <GL/glew.h>

#include <Utils/IO.hpp>
#include <Math/Vector2.hpp>
#include <Graphics/Vertex2D.hpp>

struct GLRenderGroup {
	GLuint vertexArrayID;
	GLuint vertexBufferID;
	GLuint elementBufferID;
	GLuint shaderProgramID;
	GLuint currentVertexIndex;
	GLuint currentVertexCount;
	GLuint maxVertexCount;
};

void DEBUGInitGroup(GLRenderGroup* renderGroup, GLuint vertexCount, const char* vertexShaderSource, const char* fragmentShaderSource);
void DEBUGDisposeGroup(GLRenderGroup* renderGroup);
void DEBUGDrawTexture(GLuint textureID, float32 x, float32 y, float32 width, float32 height, GLRenderGroup* renderGroup);
void DEBUGPushRect(float32 x, float32 y, float32 width, float32 height, GLRenderGroup* renderGroup);
void DEBUGRenderGroup(GLRenderGroup* group);
