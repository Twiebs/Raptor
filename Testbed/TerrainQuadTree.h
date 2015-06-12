#pragma once

#include<memory>
#include<mutex>
#include<vector>

#include<Math\Rectangle.h>
#include<Math\Vertex3D.hpp>
#include<Graphics\Mesh.h>
#include<Graphics\IRenderer.h>

#include<glm\gtc\noise.hpp>
#include<Math\Noise.h>

class TerrainQuadTree {
public:
	static float* heightmap;
	static int rootWidth;
	Rectangle bounds;

	TerrainQuadTree(Rectangle& bounds, float terrainHeight, float LODDistance, TerrainQuadTree* parent, int level);
	~TerrainQuadTree();

	bool Subdivde();
	bool Combine();
	bool LODPosition(Vector3& position);
	void Draw(std::unique_ptr<IRenderer>& renderer, std::unique_ptr<GLSLProgram>& shader);

	static void GenerateTerrainTask(float terrainX, float terrainZ, float terrainWidth, float terrainLength, float terrainHeight, TerrainQuadTree* terrain);

private:
	std::mutex mutex;
	float terrainHeight;
	float LODDistance;
	float heightScalar = 1.0f;
	float LODScalar = 0.5f;
	bool active = false;
	int level;

	int maxLevel = 6;

	Mesh* mesh;
	TerrainQuadTree* parent;
	TerrainQuadTree* topLeft;
	TerrainQuadTree* topRight;
	TerrainQuadTree* bottomLeft;
	TerrainQuadTree* bottomRight;
};