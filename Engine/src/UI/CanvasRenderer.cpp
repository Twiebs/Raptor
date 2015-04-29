#include "CanvasRenderer.h"
#include<glm/gtc/type_ptr.hpp>

CanvasRenderer::CanvasRenderer() {
	fontShader.Init("Resources/shaders/Text.vert", "resources/shaders/Text.frag");

	projectionMatrix = glm::ortho(0.0f, 1280.0f, 0.0f, 720.0f);
	glUniformMatrix4fv(fontShader.GetUniformLocation("projection"), 1, GL_FALSE, &projectionMatrix[0][0]);
}


CanvasRenderer::~CanvasRenderer() {
}

void CanvasRenderer::RenderFont(Font* font, std::string text, int x, int y, Color* color) {
	font->RenderText(&fontShader, text, x, y, 1.0f, color);
}