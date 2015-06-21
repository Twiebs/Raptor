#include "RenderSystem3D.hpp"


RenderSystem3D::RenderSystem3D(){
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
RenderSystem3D::~RenderSystem3D() {

}

bool RenderSystem3D::Startup(EntityManager* manager) {
	meshComponentID = manager->ComponentTypeOf<MeshComponent>()->index;
	modelComponentID = manager->ComponentTypeOf<ModelComponent>()->index;

	wireframeShaderID = engine->assetManager->LoadShader("Assets/shaders/wireframe.vert", "Assets/shaders/wireframe.frag");
	meshShaderID= engine->assetManager->LoadShader("Assets/shaders/Model.vert", "Assets/shaders/Model.frag");
	return true;
}
bool RenderSystem3D::Shutdown(EntityManager* manager) {
	return true;
}

void RenderSystem3D::Update(double deltaTime) {
	GLSLProgram* shader = nullptr;
	if (wireframeEnabled) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		shader = engine->assetManager->GetAsset<GLSLProgram>(wireframeShaderID);
	}
	else {
		shader = engine->assetManager->GetAsset<GLSLProgram>(meshShaderID);
	}

	shader->Use();
	GLuint mvpLoc = shader->GetUniformLocation("mvp");
	ComponentBlock* block = manager->GetComponentBlock(meshComponentID);
	if (block != nullptr) {
		uint32 count = block->Count();
		MeshComponent* mesh = (MeshComponent*)block->Get(0);
		for (uint32 i = 0; i < count; i++, mesh++) {
			glm::mat4 mvp(1.0);	//FIXME Mesh positioning broken
			glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &mvp[0][0]);
			ProcessMesh(*mesh, deltaTime);
		}
	}

	block = manager->GetComponentBlock(modelComponentID);
	if (block != nullptr) {
		uint32 count = block->Count();
		ModelComponent* model = (ModelComponent*)block->Get(0);
		for (uint32 i = 0; i < count; i++, model++) {
			ProcessModel(*model, deltaTime);
		}
	}

	if (wireframeEnabled) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

void RenderSystem3D::ProcessMesh(MeshComponent& mesh, double deltaTime) {
	uint32 diffuseNumber = 1;
	uint32 specularNumber = 1;
	//FIXME textured meshes currently broken

	//for (GLuint i = 0; i < mesh->textures.size(); i++) {
	//	std::stringstream stream;
	//	std::string number;
	//	std::string name = mesh->textures[i]->textureType;
	//	if (name == "textureDiffuse")
	//		stream << diffuseNr++;
	//	else if (name == "textureSpecular")
	//		stream << specularNr++;
	//	number = stream.str();

	//	glUniform1f(shader->GetUniformLocation(("material." + name + number).c_str()), i);
	//	glActiveTexture(GL_TEXTURE0 + i);
	//	glBindTexture(GL_TEXTURE_2D, mesh->textures[i]->id);
	//}
	//glActiveTexture(GL_TEXTURE0);


	glBindVertexArray(mesh.vertexArrayID);
	glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void RenderSystem3D::ProcessModel(ModelComponent& modelComponent, double deltaTime) {
	Model* model = engine->assetManager->GetAsset<Model>(modelComponent.id);
	if (model == nullptr) return;
	for (uint32 i = 0; i < model->meshes.size(); i++) {
		auto& mesh = model->meshes[i];
		glBindVertexArray(mesh->vertexArrayID);
		glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
}



//void GLRenderer::Draw(std::unique_ptr<Skybox>& skybox, std::unique_ptr<GLSLProgram>& shader) {
//	glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content
//	shader->Use();
//	glm::mat4 projection = camera->GetProjectionMatrix();
//	glm::mat4 view = glm::mat4(glm::mat3(camera->GetViewMatrix()));	// Remove any translation component of the view matrix
//	glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), "view"), 1, GL_FALSE, &view[0][0]);
//	glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), "projection"), 1, GL_FALSE, &projection[0][0]);
//	glBindVertexArray(skybox->vertexArrayID);
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->cubemap->id);
//	glDrawArrays(GL_TRIANGLES, 0, 36);
//	glBindVertexArray(0);
//	glDepthFunc(GL_LESS); // Set depth function back to default
//}
