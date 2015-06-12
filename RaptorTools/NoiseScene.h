#pragma once

#include<Core\IScene.h>
#include<Graphics\SpriteBatch.h>
#include<UI\Canvas.h>

#include<Math\Noise.h>
#include<Math\Random.h>


using namespace Raptor;

class NoiseSceneCanvas : public Canvas{
public:
	NoiseSceneCanvas(float viewportWidth, float viewportHeight);
	~NoiseSceneCanvas();

	void Render(float deltaTime) override;
};

class NoiseScene : public IScene, public IInputListener{
public:
	NoiseScene();
	~NoiseScene();

	void OnLoad(Engine& engine) override;
	void OnDestroy(Engine& engine) override;

	void Tick(float deltaTime) override;
	void Render(float deltaTime) override;
private:
	std::unique_ptr<SpriteBatch> batch;
	GLTexture* texture;
	GLTexture* noiseTexture;

	GLMSimplexNoise noise;
	Random rng;

	std::unique_ptr<NoiseSceneCanvas> canvas;

	bool OnKeyEvent(int keycode, bool isPressed, int mods);
	void GenerateTexture(int octaves, float frequency, float persistance);
};

