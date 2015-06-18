#include "ModelViewerScene.h"

#include<GL\glew.h>

ModelViewerScene::ModelViewerScene() {
	//glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

ModelViewerScene::~ModelViewerScene() {
}

void ModelViewerScene::OnLoad(Engine& engine) {
	engine.app->SetCursorHidden(false);
	//auto fontID = engine.assetManager->LoadFont("Resource/fonts/arial.ttf", 18);
	AssetID modelID = engine.assetManager->LoadModel("Resources/models/nanosuit/nanosuit.obj");

	engine.entityManager->CreateSystem<RenderSystem3D>();
	auto entityID = engine.entityManager->CreateEntity();
	auto entity = engine.entityManager->GetEntity(entityID);
	auto model = entity->AddComponent<ModelComponent>();
	model->id = modelID;

	camera = std::make_unique<PerspectiveCamera>(engine.app->GetWidth(), engine.app->GetHeight());
	camera->position.Set(0.0f, 7.5f, 15.0f);
}
void ModelViewerScene::Render(float deltaTime) {
	//glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glm::mat4 transform = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 8.0f, 0.0f));
	//transform = glm::rotate(transform, yRot, glm::vec3(1.0f, 0.0f, 0.0f));
	//transform = glm::rotate(transform, xRot, glm::vec3(0.0f, 1.0f, 0.0f));
	//transform = glm::translate(transform, glm::vec3(0.0f, -8.0f, 0.0f));
	//glm::mat4 mvp = camera->GetCombinedMatrix() * transform;
	//shader->Use();
	//shader->SetMatrix4("mvp", mvp);
}

void ModelViewerScene::OnDestroy(Engine& engine){

}
void ModelViewerScene::Tick(float deltaTime) {
	camera->Update();
}

bool ModelViewerScene::OnKeyEvent(int keycode, bool isPressed, int mod){
	if (!isPressed) return false;

	switch (keycode){
	case Key::F3:
		return true;
	default: return false;
	}

	return false;
}

bool ModelViewerScene::OnMouseButtonEvent(int button, double xpos, double ypos, bool isPressed, int mods){
	if (button == MouseButton::BUTTON_RIGHT){
		if (isPressed){
			isRotating = true;
			return true;
		}
		else{
			isRotating = false;
			return true;
		}
	}
	return false;
}

bool ModelViewerScene::OnCursorPosEvent(double xpos, double ypos){
	static double lastX = xpos;
	static double lastY = ypos;

	float deltaX = xpos - lastX;
	float deltaY = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	if (isRotating){
		xRot += deltaX * rotationSensitivity;
		yRot += deltaY * rotationSensitivity;
		yRot = MathUtils::Clamp(yRot, -89.0f, 89.0f);
		return true;
	}
	return false;
}