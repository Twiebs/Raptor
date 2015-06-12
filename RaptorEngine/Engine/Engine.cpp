#include "Engine.h"
#include<iostream>
#include<thread>

Engine::Engine() {
}


Engine::~Engine() {
}

void Engine::Start(IApplication* app) {
	this->app = app;
}

void Engine::LoadScene(IScene* scene) {
	if (currentScene != nullptr) {
		//TODO unload previous scene or throw error
	}

	this->currentScene = scene;
	this->currentScene->OnLoad(*this);
	Log("Loaded current scene");
}

void Engine::StartScene(IScene* scene) {

}

void Engine::Update(float deltaTime) {
	currentScene->Tick(deltaTime);
	currentScene->Render(deltaTime);
}

void Engine::Log(std::string message) {
	std::cout << message << std::endl;
}
