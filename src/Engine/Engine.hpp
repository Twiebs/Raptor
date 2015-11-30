#pragma once

#include <Core/types.h>
#include <Core/logging.h>
#include <Core/Platform.h>
#include <Core/Tasks.hpp>

#include <Engine/Assets.hpp>
#include <Engine/GFX3D.hpp>

#include <Utils/Profiler.hpp>

class Engine {
public:
	static void Init();
	static void Terminate();
	static void Update();

	static TaskManager* GetTaskManager();
	// static AssetManager* GetAssetManager();
	static AssetManifest* GetAssetManifest();

private:
	Engine();

	static TaskManager taskManager;
	static AssetManifest assetManifest;
	static AssetManager assetManager;
};


template <typename TTask, typename... TArgs>
inline void ScheduleTask(TArgs... args) {
	ScheduleTask<TTask>(&Engine::GetTaskManager()->workQueue, args...);
}


//}