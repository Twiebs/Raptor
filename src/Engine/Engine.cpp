#include "Engine.hpp"

global_variable TaskManager global_TaskManager;
global_variable Profiler global_Profiler;
global_variable AssetManifest global_AssetManifest;

void Engine::Init() {
	InitTaskManager(&global_TaskManager);
	GFX3D::Init();
}

void Engine::Terminate() {
	TerminateTaskManager(&global_TaskManager);
}

void Engine::Update() {
	FinializeCompletedTasks(&global_TaskManager.workQueue);
}

TaskManager* Engine::GetGlobalTaskManager() {
	return &global_TaskManager;
}

Profiler* Engine::GetGlobalProfiler() {
	return &global_Profiler;
}

AssetManifest* Engine::GetGlobalAssetManifest() {
	return &global_AssetManifest;
}