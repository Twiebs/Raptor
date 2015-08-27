#pragma once

#include <vector>

#include <Core/Common.hpp>
#include <GL/glew.h>

#include <Graphics/Vertex2D.hpp>

#include <Math/Vector2.hpp>
#include <Math/Matrix4.hpp>

struct DEBUGRenderGroup {
	GLuint vertexArrayID;
	GLuint vertexBufferID;
	GLuint elementBufferID;

	GLuint currentTextureID;

	U32 maxVertexCount;
	U32 currentVertexCount = 0;
	U32 drawCalls = 0;
};

void DEBUGBindGroup(DEBUGRenderGroup* group);
void DEBUGFlushGroup(DEBUGRenderGroup* group);
void DEBUGPushVertices(DEBUGRenderGroup* group, Vertex2D* vertices, uint32 count);
void DEBUGDrawTexture(DEBUGRenderGroup* group, GLuint textureID, float32 x, float32 y, float32 width, float32 height, Color color);
void DEBUGDrawTexture(DEBUGRenderGroup* group, GLuint textureID, Vector2 position, Vector2 size, Color color);
void DEBUGDrawCircle(DEBUGRenderGroup* group, float32 centerX, float32 centerY, float32 radius, uint32 numSegments);
void DEBUGFillRect(DEBUGRenderGroup* group, float32 x, float32 y, float32 width, float32 height, Color color);
void DEBUGCreateRenderGroup(DEBUGRenderGroup* group, uint32 maxVertexCount);
void DEBUGAddAttribute(DEBUGRenderGroup* group, U32 index, U32 count, GLenum type, size_t size, void* offset);

struct DEBUGBufferGroup {
	GLuint vertexArrayID;
	GLuint vertexBufferID;
	GLuint elementBufferID;

	size_t vertexTypeSize;
	GLuint maxVertexCount;
	GLuint currentVertexCount;
};

void DEBUGBindGroup(DEBUGBufferGroup* group);
void DEBUGCreateBufferGroup(DEBUGBufferGroup* group, size_t size, U32 maxVertexCount);
void DEBUGAddAttribute(DEBUGBufferGroup* group, U32 index, U32 count, GLenum type, size_t size, void* offset);
void DEBUGDrawGroup(DEBUGBufferGroup* group);
void DEBUGPushVertices(DEBUGBufferGroup* group, void* vertices, U32 count);
