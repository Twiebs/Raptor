#pragma once
#include<memory>

#include<Core\Engine.inc.h>
#include<Graphics\SceneRenderer.h>
#include<Graphics\CameraController.h>

#include<UI\Canvas.h>

class Engine;
class CameraController;

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
	SceneRenderer renderer;
	bool drawDeveloperCanvas = true;

	BaseScene();
	~BaseScene();

	Entity* AddEntity(Entity* entity);
	Light* AddLight(Light* light);
	
	bool RemoveEntity(Entity* entity);
	bool RemoveLight(Light* light);

protected:
	Canvas* canvas;
	CameraController cameraController;

	Camera* camera;

	std::vector<Light*> lights;
	std::vector<Entity*> entities;

};



