#pragma once

#include <Core\IScene.h>
#include <Core\EntityManager.hpp>
#include <Core\AssetManager.hpp>

#include <Components/Transform2D.hpp>

#include <Systems\RenderSystem2D.hpp>
#include <Assets\Font.hpp>

class ECSTestScene : public IScene{
public:
	ECSTestScene();
	~ECSTestScene();

	void OnLoad(Engine& engine) override;
	void OnDestroy(Engine& engine) override;

	void Tick(float deltaTime) override;
	void Render(float deltaTime) override;
};

