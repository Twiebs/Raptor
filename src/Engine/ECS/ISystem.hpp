#pragma once

#ifdef PROFILE
#include<chrono>
#endif

#include <bitset>
#include <vector>
#include <cstdarg>

#include <Core/Common.hpp>

#define REGISTER_SYSTEM_TAG(x) tag = #x;

class Engine;
class ECSManager;
class Entity;

//Base class for all systems that are registered with an [ECSManager]
class ISystem {
public:
	ISystem() { }
	virtual ~ISystem() { }

	virtual bool Startup 	(ECSManager* manager) = 0;
	virtual bool Shutdown (ECSManager* manager) = 0;
	virtual void Update 		(double deltaTime) 		 = 0;

protected:
	friend class ECSManager;
	ECSManager* manager;
	Engine* engine;

public:
	std::string tag;

#ifdef BENCHMARK
	double lastUpdateTime;
#endif

};
