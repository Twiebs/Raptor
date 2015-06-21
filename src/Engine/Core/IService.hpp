#pragma once

class Engine;

class IService {
public:
	virtual ~IService() { };

protected:
	friend class Engine;
	Engine* engine;
};
