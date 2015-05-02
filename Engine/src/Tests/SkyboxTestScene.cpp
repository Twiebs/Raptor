#include "SkyboxTestScene.h"

#include<GL\glew.h>

#include<vector>
#include<string>

#include<Graphics\Camera.h>
#include<Graphics\Shader.h>
#include<Graphics\Texture.h>

#include<glm\gtc\type_ptr.hpp>

#include<FreeImage.h>
#include<Core\Engine.h>

#include<glm\gtc\matrix_transform.hpp>
#include<Math\GeometryUtils.h>

#include<Tests\DefaultInputListener.h>
#include"DebugCanvas.h"

SkyboxTestScene::SkyboxTestScene() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


SkyboxTestScene::~SkyboxTestScene() {
}

void SkyboxTestScene::OnLoad(Engine* engine) {
	//Camera initalization
	camera = new PerspectiveCamera(1280, 720);
	cameraController.SetCamera(camera);
	engine->GetApp()->AddListener(&cameraController);
	renderer.SetCamera(camera);

	//Canvas initalization
	canvas = std::make_unique<DebugCanvas>(1280, 720);

	engine->GetApp()->AddListener(new DefaultInputListener(engine, this));

	std::vector<const GLchar*> faces;
	faces.push_back("Resources/skybox/space/right.png");
	faces.push_back("Resources/skybox/space/left.png");
	faces.push_back("Resources/skybox/space/top.png");
	faces.push_back("Resources/skybox/space/bottom.png");
	faces.push_back("Resources/skybox/space/front.png");
	faces.push_back("Resources/skybox/space/back.png");

	skybox = std::make_unique<Skybox>(faces);
	renderer.SetSkybox(skybox.get());
}

void SkyboxTestScene::Tick(float deltaTime) {
	cameraController.Update(deltaTime);
}

void SkyboxTestScene::Render(float deltaTime) {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	renderer.RenderScene();
	canvas->Render(deltaTime);
}

void SkyboxTestScene::OnDestroy(Engine* engine) {

}