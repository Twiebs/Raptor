#include "SceneRenderer.h"

#include<glm\gtc\type_ptr.hpp>

SceneRenderer::SceneRenderer(Camera* camera) {
	this->camera = camera;
	this->defaultShader = new ShaderProgram("Resources/shaders/lighting.vert", "Resources/shaders/lighting.frag");
}

SceneRenderer::SceneRenderer() {

}

SceneRenderer::~SceneRenderer() {

}

void SceneRenderer::SetCamera(Camera* camera) {

}



void SceneRenderer::AddLight(Light* light) {
	this->lights.push_back(light);
}

void SceneRenderer::SetSkybox(Skybox* skybox) {
	this->skybox = skybox;
	if (skyboxShader == nullptr) {
		skyboxShader = new ShaderProgram("Resources/shaders/skybox.vert", "Resources/shaders/skybox.frag");
	}
}

void SceneRenderer::ToggleWireframeEnabled() {
	SetWireframeEnabled(!wireframe);
}

void SceneRenderer::SetWireframeEnabled(bool enabled) {
	this->wireframe = enabled;
	if (enabled) {
		//glDisable(GL_CULL_FACE);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		if (wireframeShader == nullptr)
			wireframeShader = new ShaderProgram("Resources/shaders/wireframe.vert", "Resources/shaders/wireframe.frag");
	} else {
		//glEnable(GL_CULL_FACE);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

void SceneRenderer::RenderScene(){
	if (wireframe) {
		//glDisable(GL_CULL_FACE);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	if (skybox != nullptr) 
		RenderSkybox();

	ShaderProgram* shader = defaultShader;
	shader->Use();
	if (lights.size() > 0) {
		DirectionalLight* light = static_cast<DirectionalLight*>(lights[0]);
		glUniform3f(shader->GetUniformLocation("directionalLight.direction"), light->direction.x, light->direction.y, light->direction.z);
		glUniform3f(shader->GetUniformLocation("directionalLight.color"), light->color.x, light->color.y, light->color.z);
		glUniform1f(shader->GetUniformLocation("directionalLight.ambientIntensity"), light->ambientIntensity);
		glUniform1f(shader->GetUniformLocation("directionalLight.diffuseIntensity"), light->diffuseIntensity);
		glUniform1f(shader->GetUniformLocation("directionalLight.specularIntensity"), light->specularIntensity);
	}

	glUniform3f(shader->GetUniformLocation("objectColor"), 0.33f, 0.33f, 0.33f);
	glUniform3f(shader->GetUniformLocation("lightPosition"), 0, 32, 100);
	
	if (wireframe) {
		//glEnable(GL_CULL_FACE);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

void SceneRenderer::RenderEntity(Entity* entity) {
	glm::mat4 model = glm::translate(glm::mat4(1.0), entity->position.ToGLM());
	glm::mat4 mvp = camera->GetCombinedMatrix() * model;

	ShaderProgram* shader = wireframe ? wireframeShader : defaultShader;
	shader->Use();
	glUniformMatrix4fv(shader->GetUniformLocation("mvp"), 1, GL_FALSE, &mvp[0][0]);
	if (!wireframe) {
		glUniform3f(shader->GetUniformLocation("viewPositon"), camera->position.x, camera->position.y, camera->position.z);
		glUniformMatrix4fv(shader->GetUniformLocation("model"), 1, GL_FALSE, &model[0][0]);
	}

	Mesh* mesh = static_cast<Mesh*>(entity->GetComponent());
	glBindVertexArray(mesh->vertexArrayID);
	glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void SceneRenderer::RenderSkybox() {
	ShaderProgram* shader = wireframe ? wireframeShader : skyboxShader;

	glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content
	skyboxShader->Use();
	glm::mat4 projection = camera->GetProjectionMatrix();
	glm::mat4 view = glm::mat4(glm::mat3(camera->GetViewMatrix()));	// Remove any translation component of the view matrix
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader->GetProgram(), "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader->GetProgram(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	// skybox cube
	glBindVertexArray(skybox->vertexArrayID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->cubemap->textureID);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS); // Set depth function back to default
}
