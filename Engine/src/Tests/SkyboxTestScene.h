#pragma once
#include<string>
#include<Core\Scene.h>

class SkyboxTestScene : public BaseScene{
public:
	SkyboxTestScene();
	~SkyboxTestScene();

	void OnLoad(Engine* engine) override;
	void OnDestroy(Engine* engine) override;

	void Tick(float deltaTime) override;
	void Render(float deltaTime) override;
};

