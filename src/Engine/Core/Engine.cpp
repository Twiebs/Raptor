#include "Engine.hpp"


Engine::Engine() {
}


Engine::~Engine() {
}

void Engine::Create(std::string title, uint32 width, uint32 height, bool fullscreen) {
	entityManager = std::make_unique<EntityManager>();
	entityManager->engine = this;
	assetManager = std::make_unique<AssetManager>();
	assetManager->engine = this;
	input = std::make_unique<InputService>();
	taskManager = std::make_unique<TaskManager>();

#ifdef GLFW_APPLICATION
	app = std::make_unique<GLFWApplication>();
#endif
	app->Create(title.c_str(), width, height, fullscreen);
	app->RegisterInputService(input.get());
}

void Engine::Run() {
	running = true;
	static double lastTime = app->GetTime();
	static double frameTime = app->GetTime();

	while (running && !app->ShouldClose()) {
		double currentTime = app->GetTime();
		double deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		app->BeginFrame();
		Update(deltaTime);
		app->EndFrame();
	}
}

void Engine::Exit() {
	running = false;
}

//TODO abstract frame begin / end
void Engine::Update(double deltaTime) {
	glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	taskManager->Update(deltaTime);	 //Finialize any completed tasks that are now ready
	entityManager->Update(deltaTime);	//Tick the entity manager

	//These are now useless?
	currentScene->Tick(deltaTime);
	currentScene->Render(deltaTime);
}
