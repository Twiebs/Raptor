#include "VoxelChunk.h"
#include<vector>

#include<Math\MathUtils.h>
#include<Math\GeometryUtils.h>

#include<glm\glm.hpp>
#include<glm\gtc\noise.hpp>

using namespace std;

VoxelChunk::VoxelChunk(Vector3 position) {
	this->position = position;
	GenerateScalarData();
	GenerateVoxels();
}

VoxelChunk::~VoxelChunk() {

}


void VoxelChunk::GenerateScalarData() {
	static const float featureSize = 48;
	static const float worldMaxHeight = 32;
	static const float surfaceLevel = 8;

	float data[CHUNK_SIZE_X][CHUNK_SIZE_Y][CHUNK_SIZE_Z];

	for (int x = 0; x < CHUNK_SIZE_X; x++) {
		for (int z = 0; z < CHUNK_SIZE_Z; z++) {
			float value = glm::simplex(glm::vec2((x + position.x) / featureSize, (z + position.z) / featureSize));
			float height = worldMaxHeight * value;
			for (int y = 0; y < CHUNK_SIZE_Y; y++) {
				float worldYPos = y + position.y;
				data[x][y][z] = MathUtils::Clamp(height + worldYPos, -1.0f, 1.0f);

				//voxels[x][y][z] = data[x][y][z] < height;
				voxels[x][y][z] = 1;

			}
		}
	}
}

void VoxelChunk::GenerateVoxels() {
	vector<Vertex> verticies;
	vector<GLuint> indicies;

	for (int x = 0; x < CHUNK_SIZE_X; x++) {
		for (int y = 0; y < CHUNK_SIZE_Y; y++) {
			for (int z = 0; z < CHUNK_SIZE_Z; z++) {
				VoxelID id = voxels[x][y][z];
				if (id == 0) continue;	//Skip if the voxel is empty

				//Build faces if ajacent voxel is solid
				if (!IsCoordinateInChunkBounds(x, y, z + 1) || !voxels[x][y][z + 1])
					BuildFace(x, y, z, VOXEL_FRONT, verticies, indicies);
				if (!IsCoordinateInChunkBounds(x, y, z - 1) || !voxels[x][y][z - 1])
					BuildFace(x, y, z, VOXEL_BACK, verticies, indicies);
				if (!IsCoordinateInChunkBounds(x, y + 1, z) || !voxels[x][y + 1][z])
					BuildFace(x, y, z, VOXEL_TOP, verticies, indicies);
				if (!IsCoordinateInChunkBounds(x, y - 1, z) || !voxels[x][y - 1][z])
					BuildFace(x, y, z, VOXEL_BOTTOM, verticies, indicies);
				if (!IsCoordinateInChunkBounds(x - 1, y, z) || !voxels[x - 1][y][z])
					BuildFace(x, y, z, VOXEL_LEFT, verticies, indicies);
				if (!IsCoordinateInChunkBounds(x + 1, y, z) || !voxels[x + 1][y][z])
					BuildFace(x, y, z, VOXEL_RIGHT, verticies, indicies);
			}
		}
	}

	for (Vertex &vertex : verticies) {
		vertex.position.Normalize();
		vertex.position * 1;
	}

	if (verticies.size() != 0) {
		enabled = true;
		Mesh* mesh = new Mesh(verticies, indicies);
		AddComponent(mesh);
	}
}

/*Constructs Voxel Faces*/
void VoxelChunk::BuildFace(unsigned int x, unsigned int y, unsigned int z, VoxelDirection direction, vector<Vertex>& verticies, vector<unsigned int>& indicies) {
	switch (direction) {
	case VOXEL_FRONT:
		verticies.push_back(Vertex(Vector3(x - 1.0f, y - 1.0f, z + 1.0f), Vector3(0.0f, 0.0f, 1.0f)));
		verticies.push_back(Vertex(Vector3(x + 1.0f, y - 1.0f, z + 1.0f), Vector3(0.0f, 0.0f, 1.0f)));
		verticies.push_back(Vertex(Vector3(x + 1.0f, y + 1.0f, z + 1.0f), Vector3(0.0f, 0.0f, 1.0f)));
		verticies.push_back(Vertex(Vector3(x - 1.0f, y + 1.0f, z + 1.0f), Vector3(0.0f, 0.0f, 1.0f)));
		GenerateQuadIndicies(true, verticies.size(), indicies);
		break;
	case VOXEL_BACK:
		verticies.push_back(Vertex(Vector3(x - 1.0f, y - 1.0f, z - 1.0f), Vector3(0.0f, 0.0f, -1.0f)));
		verticies.push_back(Vertex(Vector3(x + 1.0f, y - 1.0f, z - 1.0f), Vector3(0.0f, 0.0f, -1.0f)));
		verticies.push_back(Vertex(Vector3(x + 1.0f, y + 1.0f, z - 1.0f), Vector3(0.0f, 0.0f, -1.0f)));
		verticies.push_back(Vertex(Vector3(x - 1.0f, y + 1.0f, z - 1.0f), Vector3(0.0f, 0.0f, -1.0f)));
		GenerateQuadIndicies(false, verticies.size(), indicies);
		break;
	case VOXEL_TOP:
		verticies.push_back(Vertex(Vector3(x - 1.0f, y + 1.0f, z + 1.0f), Vector3(0.0f, 1.0f, 0.0f)));
		verticies.push_back(Vertex(Vector3(x + 1.0f, y + 1.0f, z + 1.0f), Vector3(0.0f, 1.0f, 0.0f)));
		verticies.push_back(Vertex(Vector3(x + 1.0f, y + 1.0f, z - 1.0f), Vector3(0.0f, 1.0f, 0.0f)));
		verticies.push_back(Vertex(Vector3(x - 1.0f, y + 1.0f, z - 1.0f), Vector3(0.0f, 1.0f, 0.0f)));
		GenerateQuadIndicies(true, verticies.size(), indicies);
		break;
	case VOXEL_BOTTOM:
		verticies.push_back(Vertex(Vector3(x - 1.0f, y - 1.0f, z + 1.0f), Vector3(0.0f, -1.0f, 0.0f)));
		verticies.push_back(Vertex(Vector3(x + 1.0f, y - 1.0f, z + 1.0f), Vector3(0.0f, -1.0f, 0.0f)));
		verticies.push_back(Vertex(Vector3(x + 1.0f, y - 1.0f, z - 1.0f), Vector3(0.0f, -1.0f, 0.0f)));
		verticies.push_back(Vertex(Vector3(x - 1.0f, y - 1.0f, z - 1.0f), Vector3(0.0f, -1.0f, 0.0f)));
		GenerateQuadIndicies(false, verticies.size(), indicies);
		break;
	case VOXEL_LEFT:
		verticies.push_back(Vertex(Vector3(x - 1.0f, y - 1.0f, z - 1.0f), Vector3(-1.0f, 0.0f, 0.0f)));
		verticies.push_back(Vertex(Vector3(x - 1.0f, y - 1.0f, z + 1.0f), Vector3(-1.0f, 0.0f, 0.0f)));
		verticies.push_back(Vertex(Vector3(x - 1.0f, y + 1.0f, z + 1.0f), Vector3(-1.0f, 0.0f, 0.0f)));
		verticies.push_back(Vertex(Vector3(x - 1.0f, y + 1.0f, z - 1.0f), Vector3(-1.0f, 0.0f, 0.0f)));
		GenerateQuadIndicies(true, verticies.size(), indicies);
		break;
	case VOXEL_RIGHT:
		verticies.push_back(Vertex(Vector3(x + 1.0f, y - 1.0f, z + 1.0f), Vector3(1.0f, 0.0f, 0.0f)));
		verticies.push_back(Vertex(Vector3(x + 1.0f, y - 1.0f, z - 1.0f), Vector3(1.0f, 0.0f, 0.0f)));
		verticies.push_back(Vertex(Vector3(x + 1.0f, y + 1.0f, z - 1.0f), Vector3(1.0f, 0.0f, 0.0f)));
		verticies.push_back(Vertex(Vector3(x + 1.0f, y + 1.0f, z + 1.0f), Vector3(1.0f, 0.0f, 0.0f)));
		GenerateQuadIndicies(true, verticies.size(), indicies);
		break;
	}
}

void VoxelChunk::GenerateQuadIndicies(bool clockwise, int size, vector<unsigned int>& indicies) {
	if (clockwise) {
		indicies.push_back(size - 4);
		indicies.push_back(size - 3);
		indicies.push_back(size - 2);

		indicies.push_back(size - 2);
		indicies.push_back(size - 1);
		indicies.push_back(size - 4);
	} else {
		indicies.push_back(size - 1);
		indicies.push_back(size - 2);
		indicies.push_back(size - 3);

		indicies.push_back(size - 3);
		indicies.push_back(size - 4);
		indicies.push_back(size - 1);
	}
}

bool VoxelChunk::IsCoordinateInChunkBounds(int x, int y, int z) {
	if (x >= 0 && x < CHUNK_SIZE_X &&
		y >= 0 && y < CHUNK_SIZE_Y &&
		z >= 0 && z < CHUNK_SIZE_Z) {
		return true;
	}
	return false;
}



void VoxelChunk::Set(int x, int y, int z, VoxelID id) {
	if (x >= 0 && x < CHUNK_SIZE_X &&
		y >= 0 && y < CHUNK_SIZE_Y &&
		z >= 0 && z < CHUNK_SIZE_Z) {
		this->voxels[x][y][z] = id;
	}
	
}

void VoxelChunk::Update() {

}