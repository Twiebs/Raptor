#pragma once

#include<sstream>

#include<Core\IScene.h>
#include<Utils\FPSCamera.h>
#include <Graphics\Skybox.h>
#include<Utils\DebugCanvas.h>
#include<Graphics\GL\GLRenderer.h>

class CommonScene : public IScene, public IInputListener{
friend class DebugCanvas;
public:
	CommonScene();
	~CommonScene();

	void OnLoad(Engine& engine) override;
	void OnDestroy(Engine& engine) override;

	void Tick(float deltaTime) override;
	void Render(float deltaTime) override;

	bool OnKeyEvent(int keycode, bool isPressed, int mods) override;

protected:
	std::unique_ptr<FPSCamera> camera;
	std::unique_ptr<Skybox> skybox;
	std::unique_ptr<DebugCanvas> canvas;

	std::unique_ptr<IRenderer> renderer;

	//GLSL Shader Programs
	std::unique_ptr<GLSLProgram> wireframeShader;
	std::unique_ptr<GLSLProgram> skyboxShader;

	bool drawDevelCanvasEnabled = false;
};

