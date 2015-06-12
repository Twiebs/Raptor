#pragma once

#include<Core\IScene.h>
#include<Graphics\Model.h>
#include<Graphics\GL\GLRenderer.h>

class ModelViewerScene : public IScene , public IInputListener{
public:
	ModelViewerScene();
	~ModelViewerScene();

	void OnLoad(Engine& engine) override;
	void OnDestroy(Engine& engine) override;

	void Tick(float deltaTime) override;
	void Render(float deltaTime) override;
private:
	std::unique_ptr<Camera> camera;
	std::unique_ptr<Model> model;
	std::unique_ptr<GLRenderer> renderer;

	std::unique_ptr<GLSLProgram> shader;

	bool isRotating = false;
	float xRot = 0.0f;
	float yRot = 0.0f;
	float rotationSensitivity = 0.005f;

	Engine* engine;

	bool OnKeyEvent(int keycode, bool isPressed, int mod) override;
	bool OnMouseButtonEvent(int button, bool isPressed, int mods) override;
	bool OnCursorPosEvent(double xPos, double yPos) override;
};

