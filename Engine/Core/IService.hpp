#pragma once

class Engine;

class IService {
public:
	virtual ~IService() { };

	virtual void AddedToEngine(Engine* engine);

protected:
	friend class Engine;
	Engine* engine;
};