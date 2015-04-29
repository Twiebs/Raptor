#include "Engine.h"

#include<iostream>
#include<thread>

Engine::Engine() {

}


Engine::~Engine() {
}

void test (){

}

void Engine::LoadScene(IScene* scene) {
	if (currentScene != nullptr) {
		//TODO unload previous scene or throw error
	}

	this->currentScene = scene;
	this->currentScene->OnLoad(this);
	Log("Loaded current scene");
}

void Engine::StartScene(IScene* scene) {

}

void Engine::Exit() {
	this->isRunning = false;
}

void Engine::Update(float deltaTime) {
	currentScene->Tick(deltaTime);
	currentScene->Render(deltaTime);
}

void Engine::Log(std::string message) {
	std::cout << message << std::endl;
}
