#include "CanvasRenderer.h"
#include<glm/gtc/type_ptr.hpp>

#include<Math\Vector2.h>

CanvasRenderer::CanvasRenderer() {
	projectionMatrix = glm::ortho(0.0f, 1280.0f, 0.0f, 720.0f);

	fontShader.Init("Resources/shaders/Text.vert", "resources/shaders/Text.frag");
	fontShader.Use();
	glUniformMatrix4fv(fontShader.GetUniformLocation("projection"), 1, GL_FALSE, &projectionMatrix[0][0]);

	shader.Init("Resources/shaders/UI.vert", "Resources/shaders/UI.frag");
	shader.Use();
	glUniformMatrix4fv(shader.GetUniformLocation("mvp"), 1, GL_FALSE, &projectionMatrix[0][0]);

	InitGL();
}

void CanvasRenderer::InitGL() {
	Vector2 vertices[] {
		Vector2(0.0f, 0.0f),
		Vector2(100.0f, 0.0f),
		Vector2(100.0f, 100.0f),

		Vector2(0.0f, 0.0f),
		Vector2(100.0f, 100.0f),
		Vector2(0.0f, 100.0f)
	};

	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	glGenBuffers(1, &vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) * sizeof(Vector2), vertices, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vector2) * 6, NULL, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vector2), (GLvoid*)0);

	glBindVertexArray(0);
}


CanvasRenderer::~CanvasRenderer() {

}

void CanvasRenderer::Begin() {
	shader.Use();
	glBindVertexArray(vertexArrayID);
}

void CanvasRenderer::DrawRect(float x, float y, float width, float height, Color& color) {
	Vector2 vertices[] {
		Vector2(x, y),
		Vector2(x + width, y),
		Vector2(x + width, y + height),

		Vector2(x, y),
		Vector2(x + width, y + height),
		Vector2(x, y + height)
	};

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glUniform4f(shader.GetUniformLocation("color"), color.r, color.g, color.b, color.a);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void CanvasRenderer::Render() {
	shader.Use();
	//glUniformMatrix4fv(shader.GetUniformLocation("mvp"), 1, GL_FALSE, &projectionMatrix[0][0]);
	glBindVertexArray(vertexArrayID);

	glBindVertexArray(0);
}

void CanvasRenderer::Flush() {
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}



void CanvasRenderer::RenderFont(Font* font, std::string text, int x, int y, Color* color) {
	font->RenderText(&fontShader, text, x, y, 1.0f, color);
}