#pragma once

#include <Core/Engine.hpp>
#include <Core/IScene.h>

#include <GL/glew.h>

#include <Graphics\Color.h>
#include <Graphics\Light.h>
#include <Graphics\GL\GLSLProgram.h>
#include <UI\Canvas.h>
#include <Utils\DebugCanvas.h>

#include<Utils\FPSCamera.h>

class SpaceScene : public IScene {
public:
	SpaceScene();
	~SpaceScene();

	void OnLoad(Engine& engine) override;
	void OnDestroy(Engine& engine) override;

	void Tick(float deltaTime) override;
	void Render(float deltaTime) override;

	bool OnKeyEvent(int keycode, bool isPressed, int mods) override;
	bool OnCursorPosEvent(double xpos, double ypos) override;

	//Scene specific
	GLuint GenerateJovianTexture(int textureSize, RGBA8 baseColor, RGBA8 bandTint, float bandingArea);

private:
	std::unique_ptr<FPSCamera> camera;
	std::unique_ptr<DebugCanvas> canvas;

	std::unique_ptr<Universe> universe;
	std::vector<std::unique_ptr<CelestialBody>> bodies;
	std::unique_ptr<Skybox> skybox;

	//GL
	std::unique_ptr<GLSLProgram> terrestrialShader;
	std::unique_ptr<GLSLProgram> solarShader;
	std::unique_ptr<GLSLProgram> jovianShader;
	std::unique_ptr<GLSLProgram> wireframeShader;
	std::unique_ptr<GLSLProgram> skyboxShader;
	std::unique_ptr<GLSLProgram> modelShader;

	std::unique_ptr<Cubemap> planetTexture;

	//Debug stuff
	std::unique_ptr<Model> nanosuitModel;

	std::unique_ptr<GLRenderer> renderer;

	//bad...
	Engine* engine;

	std::unique_ptr<PointLight> light;

	bool drawDeveloperCanvas;
	bool wireframe;

	GLuint texture1D;
};

