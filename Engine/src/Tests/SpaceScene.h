#pragma once

#include<Core\Scene.h>

#include <Graphics\Camera.h>
#include <Graphics\CameraController.h>
#include <Graphics\Shader.h>
#include <Graphics\Light.h>
#include <Graphics\Texture.h>

#include<UI\Canvas.h>
#include<Graphics\SceneRenderer.h>
#include<Tests\DebugCanvas.h>

//Scene
#include<Universe\CelestialBody.h>
#include<Universe\Universe.h>

#include<GL\glew.h>

class SpaceScene : public BaseScene {
public:
	SpaceScene();
	~SpaceScene();

	void OnLoad(Engine* engine) override;
	void OnDestroy(Engine* engine) override;

	void Tick(float deltaTime) override;
	void Render(float deltaTime) override;

private:
	std::unique_ptr<Skybox> skybox;
	std::unique_ptr<Universe> universe;


	Entity* entity;
	DirectionalLight light;

	bool displayDevOverlay;
};

