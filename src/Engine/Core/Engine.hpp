#pragma once

#include <iostream>
#include <string>

#include <Core/Common.hpp>
#ifdef GLFW_APPLICATION
#include <Application/GLFWApplication.hpp>
#endif

#include <Core/IService.hpp>
#include <Core/EntityManager.hpp>
#include <Core/AssetManager.hpp>
#include <Core/TaskManager.hpp>
#include <Core/IScene.hpp>

class IScene;
class EntityManager;
class AssetManager;

class Engine {
public:
	std::unique_ptr<AssetManager> assetManager;
	std::unique_ptr<TaskManager> taskManager;
	std::unique_ptr<IApplication> app;
	std::unique_ptr<InputService> input;
	std::unique_ptr<EntityManager> entityManager;

	Engine();
	~Engine();

	void Create(std::string tile, uint32 width, uint32 height, bool fullscreen);
	void Run();
	void Update(double deltaTime);
	void Exit();

	//Creates a new scene of type T
	//Typicaly called in the main function of the application
	//Provides hands free mangment of its lifetime
	template<typename T, typename... Args>
	void CreateScene(Args... args) {
		currentScene = std::make_unique<T>(args...);
		input->AddListener(currentScene.get());
		currentScene->OnLoad(this);
	}

	//Destroys the currently active scene and remvoes it from the input processing system
	void EndScene() {
		input->RemoveListener(currentScene.get());
		currentScene.reset();
	}

private:
	friend class IApplication;
	friend class GLFWApplication;

	bool running = false;
	std::unique_ptr<IScene> currentScene;
};
