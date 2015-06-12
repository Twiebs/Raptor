#pragma once

#include<Graphics\Light.h>
#include<Graphics\GL\GLSLProgram.h>
#include<Graphics\Skybox.h>

class SceneRenderer {
public:
	SceneRenderer();
	SceneRenderer(Camera* camera);
	~SceneRenderer();

	void SetCamera(Camera* camera);

	void SetWireframeEnabled(bool enabled);
	void ToggleWireframeEnabled();

	void AddLight(Light* light);
	void SetSkybox(Skybox* skybox);

	void RenderScene();
	void RenderSkybox();
private:
	Camera* camera;

	GLSLProgram* solarShader;
	GLSLProgram* defaultShader;
	GLSLProgram* skyboxShader;
	GLSLProgram* wireframeShader = nullptr;

	Skybox* skybox;

	std::vector<Light*> lights;

	bool wireframe;
};

