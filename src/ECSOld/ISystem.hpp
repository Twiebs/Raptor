#pragma once

#ifdef PROFILE
#include<chrono>
#endif

#include <Core/Common.hpp>

#define REGISTER_SYSTEM_TAG(x) tag = #x;

class ECSManager;
//Base class for all systems that are registered with an [ECSManager]
class ISystem {
friend class ECSManager;
public:
	virtual ~ISystem() { }

	virtual bool Startup 	(ECSManager* manager) = 0;
	virtual bool Shutdown	(ECSManager* manager) = 0;
	virtual void Update		(double deltaTime)	  = 0;

protected:
	ECSManager* manager;

public:
	std::string tag;

#ifdef BENCHMARK
	double lastUpdateTime;
#endif

};
