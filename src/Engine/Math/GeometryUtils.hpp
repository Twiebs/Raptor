#pragma once

#include <vector>
#include <Math/Vector3.hpp>
#include <Graphics/Mesh.hpp>
#include <glm/gtc/noise.hpp>


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

	std::unique_ptr<Mesh> BuildCubeMesh(int segments);
	void BuildCubeFace(CubeFace direction, int segments, std::vector<Vertex3D>& verticies, std::vector<unsigned int>& indicies);
	void GenerateFaceIndicies(bool clockwise, int size, std::vector<unsigned int>& indicies);
}
