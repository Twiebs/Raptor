#pragma once

#include <Core/Engine.hpp>
#include <Core/IScene.hpp>

#include <Systems/RenderSystem3D.hpp>

#include <Components/ModelComponent.hpp>

#include <Graphics/Model.hpp>
#include <Graphics/Camera.hpp>

class ModelViewerScene : public IScene {
public:
	ModelViewerScene();
	~ModelViewerScene();

	void OnLoad(Engine* engine) override;
	void OnDestroy(Engine* engine) override;

	void Tick(float deltaTime) override;
	void Render(float deltaTime) override;

private:
	std::unique_ptr<Camera> camera;
	std::unique_ptr<Model> model;
	std::unique_ptr<GLSLProgram> shader;

	bool isRotating = false;
	float xRot = 0.0f;
	float yRot = 0.0f;
	float rotationSensitivity = 0.005f;

	bool OnKeyEvent(int keycode, bool isPressed, int mod) override;
	bool OnMouseButtonEvent(int button, double xpos, double ypos, bool isPressed, int mods) override;
	bool OnCursorPosEvent(double xPos, double yPos) override;
};
