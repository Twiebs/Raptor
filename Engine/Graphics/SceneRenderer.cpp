#include "SceneRenderer.h"

#include<glm\gtc\type_ptr.hpp>

SceneRenderer::SceneRenderer(Camera* camera) {
	this->camera = camera;
}

SceneRenderer::SceneRenderer() {
	std::vector<ImageData*> images;
	images.push_back(ResourceManager::Instance().GenerateImage(1, 1));
	images.push_back(ResourceManager::Instance().GenerateImage(1, 1));
	images.push_back(ResourceManager::Instance().GenerateImage(1, 1));
	images.push_back(ResourceManager::Instance().GenerateImage(1, 1));
	images.push_back(ResourceManager::Instance().GenerateImage(1, 1));
	images.push_back(ResourceManager::Instance().GenerateImage(1, 1));
	//planetTexture = std::make_unique<CubemapTexture>(images);

	this->defaultShader = new GLSLProgram("Resources/shaders/lighting.vert", "Resources/shaders/lighting.frag");
	this->solarShader = new GLSLProgram("Resources/shaders/Solar.vert", "Resources/shaders/Solar.frag");
}

SceneRenderer::~SceneRenderer() {

}

void SceneRenderer::SetCamera(Camera* camera) {
	this->camera = camera;
}



void SceneRenderer::AddLight(Light* light) {
	this->lights.push_back(light);
}

void SceneRenderer::SetSkybox(Skybox* skybox) {
	this->skybox = skybox;
	if (skyboxShader == nullptr) {
		skyboxShader = new GLSLProgram("Resources/shaders/skybox.vert", "Resources/shaders/skybox.frag");
	}
}

void SceneRenderer::ToggleWireframeEnabled() {
	SetWireframeEnabled(!wireframe);
}

void SceneRenderer::SetWireframeEnabled(bool enabled) {
	this->wireframe = enabled;
	if (enabled) {
		glDisable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		if (wireframeShader == nullptr)
			wireframeShader = new GLSLProgram("Resources/shaders/wireframe.vert", "Resources/shaders/wireframe.frag");
	} else {
		glEnable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

void SceneRenderer::RenderScene(){
	if (wireframe) {
		glDisable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	if (skybox != nullptr) 
		RenderSkybox();

	GLSLProgram* shader = defaultShader;
	shader->Use();
	if (lights.size() > 0) {
		DirectionalLight* light = static_cast<DirectionalLight*>(lights[0]);
		glUniform3f(shader->GetUniformLocation("directionalLight.color"), light->color.x, light->color.y, light->color.z);
		glUniform1f(shader->GetUniformLocation("directionalLight.ambientIntensity"), light->ambientIntensity);
		glUniform1f(shader->GetUniformLocation("directionalLight.diffuseIntensity"), light->diffuseIntensity);
		glUniform1f(shader->GetUniformLocation("directionalLight.specularIntensity"), light->specularIntensity);
	}

	glUniform3f(shader->GetUniformLocation("objectColor"), 0.33f, 0.33f, 0.33f);
	//We are at the center of the universe... aka our star
	glUniform3f(shader->GetUniformLocation("lightPosition"), 0, 0, 0);
	
	if (wireframe) {

	}
}

void SceneRenderer::RenderSkybox() {
	GLSLProgram* shader = wireframe ? wireframeShader : skyboxShader;

	glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content
	skyboxShader->Use();
	glm::mat4 projection = camera->GetProjectionMatrix();
	glm::mat4 view = glm::mat4(glm::mat3(camera->GetViewMatrix()));	// Remove any translation component of the view matrix
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader->GetProgram(), "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader->GetProgram(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	// skybox cube
	glBindVertexArray(skybox->vertexArrayID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->cubemap->id);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS); // Set depth function back to default
}
