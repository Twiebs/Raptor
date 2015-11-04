#pragma once

#include <Core/types.h>
#include <Core/logging.h>
#include <Core/Platform.h>
#include <Core/Tasks.hpp>

#include <Engine/Assets.hpp>
#include <Engine/GFX3D.hpp>

#include <Utils/Profiler.hpp>


//class Engine {
//public:
//	Engine();
//	~Engine();
//
//private:
//
//
//};

namespace Engine
{

void Init();
void Terminate();
void Update();

TaskManager* GetGlobalTaskManager();
Profiler* GetGlobalProfiler();
AssetManifest* GetGlobalAssetManifest();

template <typename TTask, typename... TArgs>
inline void ScheduleTask(TArgs... args) {
	ScheduleTask<TTask>(&GetGlobalTaskManager()->workQueue, args...);
}

}