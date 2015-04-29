#pragma once

#include<vector>
#include<unordered_map>

#include<Graphics\Camera.h>
#include<Graphics\Shader.h>

#include<VoxelEngine\VoxelChunk.h>

class VoxelTerrain {
public:
	VoxelTerrain();
	~VoxelTerrain();

	VoxelChunk* GetChunk(Vector3 position);

	void LoadChunk(Vector3 position);
	void UnloadChunk(Vector3 position);

	void RenderChunks(float deltaTime, Camera* camera, ShaderProgram* shader);

private:
	int loadDistance = 9;

	std::vector<VoxelChunk*> loadedChunks;
	std::vector<VoxelChunk*> chunkLoadList;
	std::vector<VoxelChunk*> chunkRenderList;
};

