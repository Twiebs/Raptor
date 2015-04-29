#pragma once

#include<stdint.h>
#include<GL\glew.h>


#include<Graphics\Mesh.h>
#include<Core\Entity.h>

#define CHUNK_SIZE_X 16
#define CHUNK_SIZE_Y 16
#define CHUNK_SIZE_Z 16

typedef uint8_t VoxelID;

enum VoxelDirection {
	VOXEL_FRONT,
	VOXEL_BACK,
	VOXEL_TOP,
	VOXEL_BOTTOM,
	VOXEL_LEFT,
	VOXEL_RIGHT
};

class VoxelChunk : public Entity{
public:
	VoxelChunk(Vector3 position);
	~VoxelChunk();

	void GenerateScalarData();
	void GenerateVoxels();

	VoxelID Get(int x, int y, int z);
	void Set(int x, int y, int z, VoxelID id);

	bool IsCoordinateInChunkBounds(int x, int y, int z);

	void Update();

private:
	VoxelID voxels[CHUNK_SIZE_X][CHUNK_SIZE_Y][CHUNK_SIZE_Z];
	bool enabled = false;

	void BuildFace(unsigned int x, unsigned int y, unsigned int z, VoxelDirection direction, std::vector<Vertex>& verticies, std::vector<unsigned int>& indicies);
	void GenerateQuadIndicies(bool clockwise, int size, std::vector<unsigned int>& indicies);
};

