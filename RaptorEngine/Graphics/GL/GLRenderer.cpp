#include "GLRenderer.h"


GLRenderer::GLRenderer(Camera* camera) : IRenderer(camera){
	wireframeShader = std::make_unique<GLSLProgram>("Resources/shaders/wireframe.vert", "Resources/shaders/wireframe.frag");
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


GLRenderer::~GLRenderer() {

}

void GLRenderer::Draw(std::unique_ptr<Model>& model, std::unique_ptr<GLSLProgram>& shader) {
	for (int i = 0; i < model->meshes.size(); i++) {
		Draw(model->meshes[i], shader);
	}
}

//Mesh
void GLRenderer::Draw(std::unique_ptr<Mesh>& mesh, std::unique_ptr<GLSLProgram>& shader) {
	if (wireframeEnabled) {
		DrawWireframe(mesh);
		return;
	}

	GLuint diffuseNr = 1;
	GLuint specularNr = 1;

	for (GLuint i = 0; i < mesh->textures.size(); i++) {
		std::stringstream stream;
		std::string number;
		std::string name = mesh->textures[i]->textureType;
		if (name == "textureDiffuse")
			stream << diffuseNr++;
		else if (name == "textureSpecular")
			stream << specularNr++;
		number = stream.str();

		glUniform1f(shader->GetUniformLocation(("material." + name + number).c_str()), i);
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, mesh->textures[i]->id);
	}
	glActiveTexture(GL_TEXTURE0);

	glBindVertexArray(mesh->vertexArrayID);
	glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

#ifdef DEBUG
	vertexCount += mesh->vertices.size();
#endif //DEBUG
}

void GLRenderer::Draw(Mesh* mesh, std::unique_ptr<GLSLProgram>& shader) {
	if (wireframeEnabled) {
		DrawWireframe(mesh);
		return;
	}

	GLuint diffuseNr = 1;
	GLuint specularNr = 1;

	for (GLuint i = 0; i < mesh->textures.size(); i++) {
		std::stringstream stream;
		std::string number;
		std::string name = mesh->textures[i]->textureType;
		if (name == "textureDiffuse")
			stream << diffuseNr++;
		else if (name == "textureSpecular")
			stream << specularNr++;
		number = stream.str();

		glUniform1f(shader->GetUniformLocation(("material." + name + number).c_str()), i);
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, mesh->textures[i]->id);
	}
	glBindVertexArray(mesh->vertexArrayID);
	glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

#ifdef DEBUG
	vertexCount += mesh->vertices.size();
#endif
}

void GLRenderer::Draw(std::unique_ptr<Skybox>& skybox, std::unique_ptr<GLSLProgram>& shader) {
	glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content
	shader->Use();
	glm::mat4 projection = camera->GetProjectionMatrix();
	glm::mat4 view = glm::mat4(glm::mat3(camera->GetViewMatrix()));	// Remove any translation component of the view matrix
	glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), "view"), 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), "projection"), 1, GL_FALSE, &projection[0][0]);
	glBindVertexArray(skybox->vertexArrayID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->cubemap->id);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS); // Set depth function back to default
}

void GLRenderer::SetWireframeEnabled(bool enabled) {
	wireframeEnabled = enabled;
}

void GLRenderer::SetTransformMatrix(glm::mat4& transform){
	this->transform = transform;
}

void GLRenderer::DrawWireframe(std::unique_ptr<Mesh>& mesh) {
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	wireframeShader->Use();
	glm::mat4 mvp = camera->GetCombinedMatrix();
	glUniformMatrix4fv(wireframeShader->GetUniformLocation("mvp"), 1, GL_FALSE, &mvp[0][0]);
	glBindVertexArray(mesh->vertexArrayID);
	glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void GLRenderer::DrawWireframe(Mesh* mesh) {
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	wireframeShader->Use();
	glm::mat4 mvp = camera->GetCombinedMatrix() * transform;
	transform = glm::mat4(1.0);
	glUniformMatrix4fv(wireframeShader->GetUniformLocation("mvp"), 1, GL_FALSE, &mvp[0][0]);
	glBindVertexArray(mesh->vertexArrayID);
	glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
