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

SkyboxTestScene::SkyboxTestScene() {
}


SkyboxTestScene::~SkyboxTestScene() {
}

void SkyboxTestScene::OnLoad(Engine* engine) {
	glViewport(0, 0, 1280, 720);
	camera = new PerspectiveCamera(1280, 720);

	cameraController.SetCamera(camera);
	engine->GetApp()->AddListener(&cameraController);

	engine->GetApp()->AddListener(new DefaultInputListener(engine, this));

	renderer.SetCamera(camera);

	std::vector<const GLchar*> faces;
	faces.push_back("Resources/skybox/space/right.png");
	faces.push_back("Resources/skybox/space/left.png");
	faces.push_back("Resources/skybox/space/top.png");
	faces.push_back("Resources/skybox/space/bottom.png");
	faces.push_back("Resources/skybox/space/front.png");
	faces.push_back("Resources/skybox/space/back.png");

	Skybox* skybox = new Skybox(faces);
	renderer.SetSkybox(skybox);
}

void SkyboxTestScene::Tick(float deltaTime) {
	cameraController.Update(deltaTime);
}

void SkyboxTestScene::Render(float deltaTime) {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	renderer.RenderScene();
}

void SkyboxTestScene::OnDestroy(Engine* engine) {

}