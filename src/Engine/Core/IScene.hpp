#pragma once

#include <Core/IInputListener.hpp>

class Engine;

class IScene : public IInputListener {
public:
	virtual ~IScene() { }

	virtual void OnLoad(Engine* engine) = 0;
	virtual void OnDestroy(Engine* engine) = 0;

	virtual void Tick(float deltaTime) = 0;
	virtual void Render(float deltaTime) = 0;

};
