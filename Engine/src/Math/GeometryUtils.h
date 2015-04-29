#pragma once

#include<Math\Vector3.h>
#include<Core\Engine.h>
#include<Graphics\Mesh.h>

namespace GeomertyUtils{

	enum CubeFace {
		CUBE_FRONT,
		CUBE_BACK,
		CUBE_TOP,
		CUBE_BOTTOM,
		CUBE_LEFT,
		CUBE_RIGHT
	};

	Vector3 PointToQuadrilateralizedSphere(Vector3 point);

	Mesh* BuildCubeMesh(int segments);
	void BuildCubeFace(CubeFace direction, int segments, std::vector<Vertex>& verticies, std::vector<unsigned int>& indicies);
	void GenerateFaceIndicies(bool clockwise, int size, std::vector<unsigned int>& indicies);

}
