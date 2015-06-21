#include "UICanvasTestScene.h"

#include<glm\matrix.hpp>
#include<UI\Label.h>

#include<Math\Vector2.h>
#include<glm\gtc\matrix_transform.hpp>
#include<glm\gtc\type_ptr.hpp>

#include <Math\Matrix4.h>
#include <Utils\DebugCanvas.h>

#include<Engine\Entity.h>

UICanvasTestScene::UICanvasTestScene() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


UICanvasTestScene::~UICanvasTestScene() {
}

void UICanvasTestScene::OnLoad(Engine& engine) {
	uiShader = new GLSLProgram("Assets/shaders/UI.vert", "Assets/shaders/UI.frag");
	canvas = std::make_unique<DebugCanvas>(1280, 720);
	font = new Font("Assets/fonts/arial.ttf", 48);
	color = new Color(1.0f, 0.0f, 0.0f, 1.0f);

	Vector2 vertices[] {
		Vector2(-1.0f, -1.0f),
		Vector2(1.0f, -1.0f),
		Vector2(1.0f, 1.0f),

		Vector2(-1.0f, -1.0f),
		Vector2(1.0f, 1.0f),
		Vector2(-1.0f, 1.0f)
	};


	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	glGenBuffers(1, &vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) * sizeof(Vector2), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vector2), (GLvoid*)0);

	glBindVertexArray(0);
}

void UICanvasTestScene::OnDestroy(Engine& engine) {

}

void UICanvasTestScene::Tick(float deltaTime) {

}
void UICanvasTestScene::Render(float deltaTime) {
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	canvas->Render(deltaTime);

	uiShader->Use();
	glm::mat4 proj = glm::ortho(0.0f, 1280.0f, 720.0f, 0.0f, 0.0f, 100.0f);
	//glm::mat4 model = glm::mat4(1.0);

	//model = glm::scale(model, glm::vec3(200, 50.0f, 50.0f));

	Matrix4 ortho = Matrix4::Ortho(0.0f, 1920, 720.0f, 0.0f, 0.0f, 100.0f, 1.0f);
	Matrix4 translate = Matrix4::Translate(0.0f, 0.0f, 0.0f);
	Matrix4 scale = Matrix4::Scale(0.5f, 0.5f, 0.5f);
	Matrix4 rotate = Matrix4::Rotate(0.75f, 0.0f, 0.0f);

	//Matrix4 mvp = ortho;
	//glm::mat4 mvp = proj * model;

	Matrix4 model = Matrix4::Identity();
	Matrix4 mvp = scale;

	//glUniformMatrix4fv(uiShader->GetUniformLocation("mvp"), 1, GL_FALSE, &mvp[0][0]);
	//glBindVertexArray(vertexArrayID);
	//glDrawArrays(GL_TRIANGLES, 0, 6);
	//glBindVertexArray(0);
}
