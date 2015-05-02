#include "SpaceScene.h"

#include<Tests\DefaultInputListener.h>
#include<Math\GeometryUtils.h>
#include<VoxelEngine\VoxelChunk.h>
#include<UI\Table.h>

#include<Core\Engine.h>



SpaceScene::SpaceScene() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


SpaceScene::~SpaceScene() {
	
}


void SpaceScene::OnLoad(Engine* engine) {
	//Camera initalization

	camera = new PerspectiveCamera(1280.0f, 720.0f);
	cameraController.SetCamera(camera);
	engine->GetApp()->AddListener(&cameraController);
	renderer.SetCamera(camera);

	//Create a light to add to the renderer
	light.direction.Set(1.0f, -1.0f, 0.0f);
	light.color.Set(1.0f, 1.0f, 1.0f);
	light.ambientIntensity = 0.2f;
	light.diffuseIntensity = 0.5f;
	light.specularIntensity = 0.5f;
	renderer.AddLight(&light);

	canvas = new DebugCanvas(1280, 720);
	static_cast<DebugCanvas*>(canvas)->camera = camera;

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

	universe = std::make_unique<Universe>(0);
	universe->CreateCelestialBody(STAR, 3);
	universe->CreateCelestialBody(TERRESTRIAL, 7);

	entity = new Entity();
	entity->AddComponent(GeomertyUtils::BuildCubeMesh(1));
}

void SpaceScene::OnDestroy(Engine* engine) {

}

void SpaceScene::Tick(float deltaTime) {
	cameraController.Update(deltaTime);
	canvas->Update(deltaTime);
	universe->Step(deltaTime);
}

void SpaceScene::Render(float deltaTime) {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	renderer.RenderScene();

	for(std::unique_ptr<CelestialBody>& body : universe->bodies) {
		renderer.RenderEntity(body.get());
	}

	renderer.RenderEntity(entity);


	if (drawDeveloperCanvas) {
		canvas->Render(deltaTime);
	}
}
