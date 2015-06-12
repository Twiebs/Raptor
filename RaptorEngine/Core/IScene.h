#pragma once
class Engine;

class IScene {
public:
	IScene();
	~IScene();

	virtual void OnLoad(Engine& engine) = 0;
	virtual void OnDestroy(Engine& engine) = 0;

	virtual void Tick(float deltaTime) = 0;
	virtual void Render(float deltaTime) = 0;
};
