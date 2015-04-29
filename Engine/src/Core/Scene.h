#pragma once

#include<Core\Entity.h>

#include<UI\Canvas.h>
#include<Graphics\CameraController.h>
#include<Graphics\Light.h>
#include<Graphics\SceneRenderer.h>

class Engine;

class IScene {
public:
	IScene();
	~IScene();

	virtual void OnLoad(Engine* engine) = 0;
	virtual void OnDestroy(Engine* engine) = 0;

	virtual void Tick(float deltaTime) = 0;
	virtual void Render(float deltaTime) = 0;
};

class BaseScene : public IScene{
public:
	SceneRenderer* renderer;

	BaseScene();
	~BaseScene();

	Entity* AddEntity(Entity* entity);
	Light* AddLight(Light* light);
	
	bool RemoveEntity(Entity* entity);
	bool RemoveLight(Light* light);

protected:
	Canvas* canvas;

	Camera* camera;
	CameraController* cameraController;

	std::vector<Light*> lights;
	std::vector<Entity*> entities;
};



