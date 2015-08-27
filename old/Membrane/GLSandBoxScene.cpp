#include "GLSandBoxScene.h"


GLSandBoxScene::GLSandBoxScene() {
}


GLSandBoxScene::~GLSandBoxScene() {
}

void GLSandBoxScene::OnLoad(Engine& engine) {
	CommonScene::OnLoad(engine);
	nanosuitModel = std::move(ResourceManager::LoadModel("Assets/models/nanosuit/nanosuit.obj"));
	modelShader = std::make_unique<GLSLProgram>("Assets/shaders/Model.vert", "Assets/shaders/Model.frag");
}

void GLSandBoxScene::Render(float deltaTime) {
	CommonScene::Render(deltaTime);
	
	renderer->Draw(nanosuitModel, wireframeShader);
}
