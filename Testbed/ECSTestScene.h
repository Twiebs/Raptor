#pragma once

#include <Core\IScene.h>
#include <ECS\EntityManager.hpp>

#include <ECS\Systems\RenderSystem2D.hpp>
#include <UI\Font.h>

class ECSTestScene : public IScene{
public:
	ECSTestScene();
	~ECSTestScene();

	void OnLoad(Engine& engine) override;
	void OnDestroy(Engine& engine) override;

	void Tick(float deltaTime) override;
	void Render(float deltaTime) override;

private:
	EntityManager* entityManager;
};

