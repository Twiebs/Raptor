#pragma once

#include<Core\Entity.h>

#include<Graphics\Light.h>
#include<Graphics\Shader.h>
#include<Graphics\Skybox.h>

class SceneRenderer {
public:
	SceneRenderer(Camera* camera);
	~SceneRenderer();

	void SetWireframeEnabled(bool enabled);
	void ToggleWireframeEnabled();

	void AddLight(Light* light);
	void SetSkybox(Skybox* skybox);

	void RenderScene();
	void RenderSkybox();
	void RenderEntity(Entity* entity);

private:
	Camera* camera;

	ShaderProgram* defaultShader;
	ShaderProgram* skyboxShader;
	ShaderProgram* wireframeShader = nullptr;

	Skybox* skybox;
	std::vector<Light*> lights;

	bool wireframe;
};

