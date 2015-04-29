#include "SpaceScene.h"

#include<Core\EntityFactory.h>
#include<Tests\DefaultInputListener.h>

#include<Math\GeometryUtils.h>

#include<VoxelEngine\VoxelChunk.h>

#include<UI\Table.h>

SpaceScene::SpaceScene() {
}


SpaceScene::~SpaceScene() {
	
}


void SpaceScene::OnLoad(Engine* engine) {
	camera = new PerspectiveCamera(1280, 720);
	cameraController = new CameraController(camera, &engine->inputProcessor);

	renderer = new SceneRenderer(camera);
	renderer->SetWireframeEnabled(false);

	light = new DirectionalLight();
	light->direction.Set(1.0f, -1.0f, 0.0f);
	light->color.Set(1.0f, 1.0f, 1.0f);
	light->ambientIntensity = 0.2f;
	light->diffuseIntensity = 0.5f;
	light->specularIntensity = 0.5f;

	renderer->AddLight(light);

	canvas = new DebugCanvas(1280, 720);
	static_cast<DebugCanvas*>(canvas)->camera = camera;

	IInputListener* listener = new DefaultInputListener(engine, this);
	engine->inputProcessor.AddListener(listener);

	std::vector<const GLchar*> faces;
	faces.push_back("Assets/skybox/space/right.png");
	faces.push_back("Assets/skybox/space/left.png");
	faces.push_back("Assets/skybox/space/top.png");
	faces.push_back("Assets/skybox/space/bottom.png");
	faces.push_back("Assets/skybox/space/front.png");
	faces.push_back("Assets/skybox/space/back.png");

	Skybox* skybox = new Skybox(faces);
	renderer->SetSkybox(skybox);

	universe = new Universe(0);
	bodies.push_back(universe->CreateCelestialBody(STAR, 4));
	bodies.push_back(universe->CreateCelestialBody(TERRESTRIAL, 4));
}

void SpaceScene::OnDestroy(Engine* engine) {

}

void SpaceScene::Tick(float deltaTime) {
	cameraController->Update(deltaTime);
	universe->Step(deltaTime);
}

void SpaceScene::Render(float deltaTime) {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	renderer->RenderScene();

	for (CelestialBody* body : bodies) {
		renderer->RenderEntity(body);
	}

	canvas->Draw(deltaTime);
}
