#include "SpaceScene.h"

#include<Core\EntityFactory.h>
#include<Tests\DefaultInputListener.h>
#include<Math\GeometryUtils.h>
#include<VoxelEngine\VoxelChunk.h>
#include<UI\Table.h>

#include<Core\Engine.h>



SpaceScene::SpaceScene() {

}


SpaceScene::~SpaceScene() {
	
}


void SpaceScene::OnLoad(Engine* engine) {
	//Initalize camera and controller
	camera = new PerspectiveCamera(1280, 720);
	cameraController.SetCamera(camera);
	engine->GetApp()->AddListener(&cameraController);

	//Tell the renderer which camera to project and set some configuration
	renderer.SetCamera(camera);
	renderer.SetWireframeEnabled(false);

	//Create a light to add to the renderer
	light.direction.Set(1.0f, -1.0f, 0.0f);
	light.color.Set(1.0f, 1.0f, 1.0f);
	light.ambientIntensity = 0.2f;
	light.diffuseIntensity = 0.5f;
	light.specularIntensity = 0.5f;
	renderer.AddLight(&light);

	canvas = new DebugCanvas(1280, 720);
	static_cast<DebugCanvas*>(canvas)->camera = camera;

	IInputListener* listener = new DefaultInputListener(engine, this);
	engine->GetApp()->AddListener(listener);

	std::vector<const GLchar*> faces;
	faces.push_back("Assets/skybox/space/right.png");
	faces.push_back("Assets/skybox/space/left.png");
	faces.push_back("Assets/skybox/space/top.png");
	faces.push_back("Assets/skybox/space/bottom.png");
	faces.push_back("Assets/skybox/space/front.png");
	faces.push_back("Assets/skybox/space/back.png");

	Skybox* skybox = new Skybox(faces);
	renderer.SetSkybox(skybox);

	bodies.push_back(universe.CreateCelestialBody(STAR, 4));
	bodies.push_back(universe.CreateCelestialBody(TERRESTRIAL, 4));
}

void SpaceScene::OnDestroy(Engine* engine) {

}

void SpaceScene::Tick(float deltaTime) {
	cameraController.Update(deltaTime);
	universe.Step(deltaTime);
}

void SpaceScene::Render(float deltaTime) {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	renderer.RenderScene();

	for (CelestialBody* body : bodies) {
		renderer.RenderEntity(body);
	}

	canvas->Draw(deltaTime);
}
