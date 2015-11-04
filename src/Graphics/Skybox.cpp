#include "Skybox.hpp"
#include <Math/Vector3.hpp>

static const V3 CUBEMAP_VERTCES[] = {
	V3(-1.0f,  1.0f, -1.0f),
	V3(-1.0f, -1.0f, -1.0f),
	V3(1.0f, -1.0f, -1.0f),
	V3(1.0f, -1.0f, -1.0f),
	V3(1.0f,  1.0f, -1.0f),
	V3(-1.0f,  1.0f, -1.0f),

	V3(-1.0f, -1.0f,  1.0f),
	V3(-1.0f, -1.0f, -1.0f),
	V3(-1.0f,  1.0f, -1.0f),
	V3(-1.0f,  1.0f, -1.0f),
	V3(-1.0f,  1.0f,  1.0f),
	V3(-1.0f, -1.0f,  1.0f),

	V3(1.0f, -1.0f, -1.0f),
	V3(1.0f, -1.0f,  1.0f),
	V3(1.0f,  1.0f,  1.0f),
	V3(1.0f,  1.0f,  1.0f),
	V3(1.0f,  1.0f, -1.0f),
	V3(1.0f, -1.0f, -1.0f),

	V3(-1.0f, -1.0f,  1.0f),
	V3(-1.0f,  1.0f,  1.0f),
	V3(1.0f,  1.0f,  1.0f),
	V3(1.0f,  1.0f,  1.0f),
	V3(1.0f, -1.0f,  1.0f),
	V3(-1.0f, -1.0f,  1.0f),

	V3(-1.0f,  1.0f, -1.0f),
	V3(1.0f,  1.0f, -1.0f),
	V3(1.0f,  1.0f,  1.0f),
	V3(1.0f,  1.0f,  1.0f),
	V3(-1.0f,  1.0f,  1.0f),
	V3(-1.0f,  1.0f, -1.0f),

	V3(-1.0f, -1.0f, -1.0f),
	V3(-1.0f, -1.0f,  1.0f),
	V3(1.0f, -1.0f, -1.0f),
	V3(1.0f, -1.0f, -1.0f),
	V3(-1.0f, -1.0f,  1.0f),
	V3(1.0f, -1.0f,  1.0f)
};


void CreateSkyboxVertices (GLuint* vao, GLuint* vbo) {
	glGenVertexArrays(1, vao);
	glGenBuffers(1, vbo);

	glBindVertexArray(*vao);
	glBindBuffer(GL_ARRAY_BUFFER, *vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(CUBEMAP_VERTCES), CUBEMAP_VERTCES, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(V3), (GLvoid*)0);
	glBindVertexArray(0);
}

void DrawSkybox (GLuint vertexArray, GLuint cubemapID, const Shader& shader) {
	glDepthMask(GL_FALSE);
	glUseProgram(shader.id);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapID);
	glBindVertexArray(vertexArray);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthMask(GL_TRUE);
}
