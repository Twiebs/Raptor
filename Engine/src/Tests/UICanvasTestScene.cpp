#include "UICanvasTestScene.h"

#include<glm\matrix.hpp>
#include<UI\Label.h>

UICanvasTestScene::UICanvasTestScene() {
}


UICanvasTestScene::~UICanvasTestScene() {
}

void UICanvasTestScene::OnLoad(Engine* engine) {
	shader = new ShaderProgram("Resources/shaders/Text.vert", "Resources/shaders/Text.frag");
	canvas = new Canvas(1280, 720);
	font = new Font("Resources/fonts/arial.ttf", 48);

	color = new Color(1.0f, 0.0f, 0.0f);
}

void UICanvasTestScene::OnDestroy(Engine* engine) {

}

void UICanvasTestScene::Tick(float deltaTime) {

}
void UICanvasTestScene::Render(float deltaTime) {
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 projection = glm::ortho(0, 1280, 0, 720);
	glUniformMatrix4fv(shader->GetUniformLocation("projection"), 1, GL_FALSE, &projection[0][0]);
	font->RenderText(shader, "Test", 100, 100, 1.0f, color);
}
