#pragma once
#include<string>
#include<Core\Scene.h>

using namespace std;

class SkyboxTestScene : public BaseScene{
public:
	SkyboxTestScene();
	~SkyboxTestScene();

	void OnLoad(Engine* engine) override;
	void OnDestroy(Engine* engine) override;

	void Tick(float deltaTime) override;
	void Render(float deltaTime) override;

private:
	std::unique_ptr<Skybox> skybox;
	std::unique_ptr<Canvas> canvas;
};

