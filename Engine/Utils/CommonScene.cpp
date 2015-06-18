#include "CommonScene.h"


CommonScene::CommonScene() {
}


CommonScene::~CommonScene() {
}


void CommonScene::OnLoad(Engine& engine) {
	camera = std::make_unique<FPSCamera>(engine.app->GetWidth(), engine.app->GetHeight());
	camera->position.Set(0.0f, 0.0f, 200.0f);
	engine.input->AddListener(camera.get());
	engine.input->AddListener(this);

	canvas = std::make_unique<DebugCanvas>(engine.app->GetWidth(), engine.app->GetHeight());
	canvas->camera = camera.get();

	std::vector<const GLchar*> faces;
	faces.push_back("Resources/skybox/islands/right.jpg");
	faces.push_back("Resources/skybox/islands/left.jpg");
	faces.push_back("Resources/skybox/islands/top.jpg");
	faces.push_back("Resources/skybox/islands/bottom.jpg");
	faces.push_back("Resources/skybox/islands/back.jpg");
	faces.push_back("Resources/skybox/islands/front.jpg");
	skybox = std::make_unique<Skybox>(faces);
	//skyboxShader = std::make_unique<GLSLProgram>("Resources/shaders/skybox.vert", "Resources/shaders/skybox.frag");
	//wireframeShader = std::make_unique<GLSLProgram>("Resources/shaders/wireframe.vert", "Resources/shaders/wireframe.frag");
}
void CommonScene::OnDestroy(Engine& engine) {

}

void CommonScene::Tick(float deltaTime) {
	camera->Update(deltaTime);
	canvas->Update(this, deltaTime);
}
void CommonScene::Render(float deltaTime) {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//renderer->Draw(skybox, skyboxShader);
	
	if (drawDevelCanvasEnabled)
		canvas->Render(deltaTime);
}


bool CommonScene::OnKeyEvent(int keycode, bool isPressed, int mods) {
	//Not interesed in release events
	if (!isPressed) {
		return false;
	}

	switch (keycode) {
	case Key::ESCAPE:
		return true;
	case Key::F3:
		return true;
	case Key::GRAVE_ACCENT:
		drawDevelCanvasEnabled = !drawDevelCanvasEnabled;
		return true;
	}

	return false;
}



