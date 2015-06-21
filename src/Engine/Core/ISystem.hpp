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
class EntityManager;
class Entity;

//Base class for all systems that are registered with an [EntityManager]
class ISystem {
public:
	ISystem() { }
	virtual ~ISystem() { }

	virtual bool Startup 	(EntityManager* manager) = 0;
	virtual bool Shutdown (EntityManager* manager) = 0;
	virtual void Update 	(double deltaTime) = 0;

protected:
	friend class EntityManager;
	EntityManager* manager;
	Engine* engine;

public:
	std::string tag;

#ifdef BENCHMARK
	double lastUpdateTime;
#endif

};
