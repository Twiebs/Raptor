#pragma once

#include<memory>
#include<vector>

#include<Math\Rectangle.h>
#include<Graphics\Mesh.h>
#include<Graphics\IRenderer.h>

class LODQuadTree {
public:
	static int rootWidth;
	static int maxLevel;
	Rectangle bounds;

	std::unique_ptr<LODQuadTree> topLeft;
	std::unique_ptr<LODQuadTree> topRight;
	std::unique_ptr<LODQuadTree> bottomLeft;
	std::unique_ptr<LODQuadTree> bottomRight;
	LODQuadTree* parent;

	LODQuadTree(Rectangle& bounds, float LODHeight, LODQuadTree* parent);
	~LODQuadTree();

	float GetHeight(float x, float z);
	void BuildMesh();
	bool Subdivde();
	bool Combine();
	bool LODPosition(Vector3& position);
	void Draw(std::unique_ptr<IRenderer>& renderer, std::unique_ptr<GLSLProgram>& shader);

private:
	bool active = false;
	int meshSize = 32;
	float heightScalar = 0.5f;
	float LODHeight;

	std::unique_ptr<Mesh> mesh;
};