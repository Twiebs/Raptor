#pragma once


#include <Core/InputService.hpp>
#include <Core/IScene.hpp>
#include <UI/Canvas.hpp>
#include <Core/Engine.hpp>

#include <Math/Noise.hpp>
#include <Math/Random.hpp>

#include <Graphics/Color.hpp>
#include <Assets/Pixmap.hpp>
#include <Graphics/GL/GLTexture.hpp>

#include <Components/SpriteComponent.hpp>

#include <Systems/RenderSystem2D.hpp>

#include <glm/gtc/matrix_transform.hpp>

class VerticalLayoutGroup {
public:
	VerticalLayoutGroup(EntityManager* manager);
	~VerticalLayoutGroup();

	void AddEntity(EntityID id);
	bool InputEvent(double cursorX, double cursorY);

	//Remove enttity
	uint16 x, y;
	uint16 width, height;

private:
	EntityManager* manager;
	uint32 textComponentID;

	std::vector<EntityID> entities;
};

class NoiseScene : public IScene {
public:
	NoiseScene();
	~NoiseScene();

	void OnLoad(Engine* engine) override;
	void OnDestroy(Engine* engine) override;

	void Tick(float deltaTime) override;
	void Render(float deltaTime) override;
private:
	GLMSimplexNoise noise;
	Random rng;
	GLuint noiseTextureID;

	VerticalLayoutGroup* group;

	EntityID seedText;
	EntityID octavesText;
	EntityID frequencyText;

	Engine* engine;

	bool OnKeyEvent(int keycode, bool isPressed, int mods) final;
	bool OnCursorPosEvent(double xpos, double ypos) final;
	bool OnMouseButtonEvent(int button, double xpos, double ypos, bool isPressed, int mods) final;


	void GenerateTexture(uint32 seed, int octaves, float frequency, float persistance);
};
