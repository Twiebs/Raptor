#include <Graphics/DEBUGRenderer.hpp>

void DEBUGInitGroup(GLRenderGroup* renderGroup, GLuint vertexCount, const char* vertexShaderSource, const char* fragmentShaderSource) {

	glGenVertexArrays(1, &renderGroup->vertexArrayID);
	glBindVertexArray(renderGroup->vertexArrayID);

	glGenBuffers(1, &renderGroup->vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, renderGroup->vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex2D) * vertexCount, NULL, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (GLvoid*)offsetof(Vertex2D, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (GLvoid*)offsetof(Vertex2D, uv));
	glBindBuffer(GL_ARRAY_BUFFER, 0);

//	uint32 indexCount = (vertexCount / 4) * 6;
//	glGenBuffers(1, &renderGroup->elementBufferID);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderGroup->elementBufferID);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32) * indexCount, indices, GL_DYNAMIC_DRAW);

	glBindVertexArray(0);
	renderGroup->shaderProgramID = DEBUGLoadShaderFromSource(vertexShaderSource, fragmentShaderSource);
}

void DEBUGDisposeGroup(GLRenderGroup* renderGroup) {
	glDeleteVertexArrays(1, &renderGroup->shaderProgramID);
	glDeleteBuffers(1, &renderGroup->shaderProgramID);
	glDeleteBuffers(1, &renderGroup->shaderProgramID);
	glDeleteShader(renderGroup->shaderProgramID);
}

void DEBUGDrawTexture(GLuint textureID, float x, float y, float width, float height, GLRenderGroup* renderGroup) {
	Vector2 vectorVerts[] {
		Vector2(x, y),
		Vector2(0.0, 0.0),

		Vector2(x + width, y),
		Vector2(1.0, 0.0),

		Vector2(x + width, y + height),
		Vector2(1.0, 1.0),

		Vector2(x, y + height),
		Vector2(0.0, 1.0)
	};

	glBindBuffer(GL_ARRAY_BUFFER, renderGroup->vertexBufferID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex2D) * 4, (GLvoid*)vectorVerts);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(renderGroup->vertexArrayID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glBindVertexArray(renderGroup->vertexArrayID);
}

void DEBUGPushRect(float x, float y, float width, float height, GLRenderGroup* renderGroup) {
	Vector2 vectorVerts[] {
		Vector2(x, y),
		Vector2(0.0, 0.0),

		Vector2(x + width, y),
		Vector2(1.0, 0.0),

		Vector2(x + width, y + height),
		Vector2(1.0, 1.0),

		Vector2(x, y + height),
		Vector2(0.0, 1.0)
	};

	glBindBuffer(GL_ARRAY_BUFFER, renderGroup->vertexBufferID);
	glBufferSubData(GL_ARRAY_BUFFER, renderGroup->currentVertexCount * sizeof(Vertex2D), sizeof(Vertex2D) * 4, (GLvoid*)vectorVerts);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	renderGroup->currentVertexCount += 4;

//	glBindVertexArray(renderGroup->vertexArrayID);
//	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//	glBindVertexArray(0);
}

void DEBUGRenderGroup(GLRenderGroup* renderGroup) {
	glUseProgram(renderGroup->shaderProgramID);
	glBindVertexArray(renderGroup->vertexArrayID);
	glDrawArrays(GL_TRIANGLES, 0, renderGroup->currentVertexCount);
	glBindVertexArray(0);
}

