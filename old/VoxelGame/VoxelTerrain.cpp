#include "VoxelTerrain.h"

VoxelTerrain::VoxelTerrain() {
	loadedChunks.reserve(loadDistance * loadDistance * loadDistance);
}


VoxelTerrain::~VoxelTerrain() {
}

VoxelChunk* VoxelTerrain::GetChunk(Vector3 position) {
	int x = position.x / CHUNK_SIZE_X;
	int y = position.y / CHUNK_SIZE_Y;
	int z = position.z / CHUNK_SIZE_Z;

	int index = (loadDistance*loadDistance * z) + (loadDistance * y) + x;
	return loadedChunks[index];
}

void VoxelTerrain::LoadChunk(Vector3 position) {
	VoxelChunk* chunk = new VoxelChunk(position);
	int x = position.x / CHUNK_SIZE_X;
	int y = position.y / CHUNK_SIZE_Y;
	int z = position.z / CHUNK_SIZE_Z;

	int index = (loadDistance*loadDistance * z) + (loadDistance * y) + x;
	//loadedChunks.insert(loadedChunks.begin() + index, chunk);
	loadedChunks.push_back(chunk);
}

void VoxelTerrain::UnloadChunk(Vector3 position) {
	

}

void VoxelTerrain::RenderChunks(float deltaTime, Camera* camera, GLSLProgram* shader) {
	for (VoxelChunk* chunk : loadedChunks) {
		glm::vec3 pos = chunk->position.ToGLM();
		glm::mat4 model = glm::translate(glm::mat4(1.0), pos);
		glm::mat4 mvp = camera->GetCombinedMatrix() * model;

		glUniformMatrix4fv(shader->GetUniformLocation("mvp"), 1, GL_FALSE, &mvp[0][0]);
		glUniformMatrix4fv(shader->GetUniformLocation("model"), 1, GL_FALSE, &model[0][0]);

	}
}