#include "Engine.hpp"

TaskManager Engine::taskManager;
AssetManifest Engine::assetManifest;
AssetManager Engine::assetManager;

void Engine::Init() {
	assetManifest.Deserialize();
	assetManager.manifest = &assetManifest;
	InitTaskManager(&taskManager);
	GFX3D::Init();
}

void Engine::Terminate() {
	TerminateTaskManager(&taskManager);
}

void Engine::Update() {
	FinializeCompletedTasks(&taskManager.workQueue);
}

//AssetManager* Engine::GetAssetManager() {
//	return &assetManager;
//}

TaskManager* Engine::GetTaskManager() {
	return &taskManager;
}

AssetManifest* Engine::GetAssetManifest() {
	return &assetManifest;
}