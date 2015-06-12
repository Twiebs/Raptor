#include "GeometryUtils.h"

#include<glm\gtc\noise.hpp>

#include<vector>

Vector3 GeomertyUtils::PointToQuadrilateralizedSphere(Vector3 point) {
	float x2 = point.x * point.x;
	float y2 = point.y * point.y;
	float z2 = point.z * point.z;

	float dx = point.x * sqrtf((1.0f) - (y2 / 2.0f) - (z2 / 2.0f) + ((y2 * z2) / 3.0f));
	float dy = point.y * sqrtf((1.0f) - (z2 / 2.0f) - (x2 / 2.0f) + ((z2 * x2) / 3.0f));
	float dz = point.z * sqrtf((1.0f) - (x2 / 2.0f) - (y2 / 2.0f) + ((x2 * y2) / 3.0f));

	return Vector3(dx, dy, dz);
}

std::unique_ptr<Mesh> GeomertyUtils::BuildCubeMesh(int segments) {
	std::vector<Vertex3D> verticies;
	std::vector<unsigned int> indicies;

	for (int i = 0; i < 6; i++) {
		BuildCubeFace((CubeFace)i, segments, verticies, indicies);
	}

	return std::make_unique<Mesh>(verticies, indicies);
}


void GeomertyUtils::BuildCubeFace(CubeFace direction, int segments, std::vector<Vertex3D>& verticies, std::vector<unsigned int>& indicies) {
	float offset = (2.0f / (float)segments);
	
	for (int x = 0; x < segments; x++) {
		for (int y = 0; y < segments; y++) {

				float i = (x * offset) - 1.0f;
				float j = (y * offset) - 1.0f;
				float disp = offset;

				switch (direction) {
				case CUBE_FRONT:
					verticies.push_back(Vertex3D(Vector3(i - 0.0f, j - 0.0f, 1.0f), Vector3(0.0f, 0.0f, 1.0f)));
					verticies.push_back(Vertex3D(Vector3(i + disp, j - 0.0f, 1.0f), Vector3(0.0f, 0.0f, 1.0f)));
					verticies.push_back(Vertex3D(Vector3(i + disp, j + disp, 1.0f), Vector3(0.0f, 0.0f, 1.0f)));
					verticies.push_back(Vertex3D(Vector3(i - 0.0f, j + disp, 1.0f), Vector3(0.0f, 0.0f, 1.0f)));
					GeomertyUtils::GenerateFaceIndicies(true, verticies.size(), indicies);
					break;
				case CUBE_BACK:
					verticies.push_back(Vertex3D(Vector3(i - 0.0f, j - 0.0f, -1.0f), Vector3(0.0f, 0.0f, -1.0f)));
					verticies.push_back(Vertex3D(Vector3(i + disp, j - 0.0f, -1.0f), Vector3(0.0f, 0.0f, -1.0f)));
					verticies.push_back(Vertex3D(Vector3(i + disp, j + disp, -1.0f), Vector3(0.0f, 0.0f, -1.0f)));
					verticies.push_back(Vertex3D(Vector3(i - 0.0f, j + disp, -1.0f), Vector3(0.0f, 0.0f, -1.0f)));
					GeomertyUtils::GenerateFaceIndicies(false, verticies.size(), indicies);
					break;
				case CUBE_TOP:
					verticies.push_back(Vertex3D(Vector3(i - 0.0f, 1.0f, j + disp), Vector3(0.0f, 1.0f, 0.0f)));
					verticies.push_back(Vertex3D(Vector3(i + disp, 1.0f, j + disp), Vector3(0.0f, 1.0f, 0.0f)));
					verticies.push_back(Vertex3D(Vector3(i + disp, 1.0f, j - 0.0f), Vector3(0.0f, 1.0f, 0.0f)));
					verticies.push_back(Vertex3D(Vector3(i - 0.0f, 1.0f, j - 0.0f), Vector3(0.0f, 1.0f, 0.0f)));
					GeomertyUtils::GenerateFaceIndicies(true, verticies.size(), indicies);
					break;
				case CUBE_BOTTOM:
					verticies.push_back(Vertex3D(Vector3(i - 0.0f, -1.0f, j + disp), Vector3(0.0f, -1.0f, 0.0f)));
					verticies.push_back(Vertex3D(Vector3(i + disp, -1.0f, j + disp), Vector3(0.0f, -1.0f, 0.0f)));
					verticies.push_back(Vertex3D(Vector3(i + disp, -1.0f, j - 0.0f), Vector3(0.0f, -1.0f, 0.0f)));
					verticies.push_back(Vertex3D(Vector3(i - 0.0f, -1.0f, j - 0.0f), Vector3(0.0f, -1.0f, 0.0f)));
					GeomertyUtils::GenerateFaceIndicies(false, verticies.size(), indicies);
					break;
				case CUBE_LEFT:
					verticies.push_back(Vertex3D(Vector3(-1.0f, i - 0.0f, j - 0.0f), Vector3(-1.0f, 0.0f, 0.0f)));
					verticies.push_back(Vertex3D(Vector3(-1.0f, i - 0.0f, j + disp), Vector3(-1.0f, 0.0f, 0.0f)));
					verticies.push_back(Vertex3D(Vector3(-1.0f, i + disp, j + disp), Vector3(-1.0f, 0.0f, 0.0f)));
					verticies.push_back(Vertex3D(Vector3(-1.0f, i + disp, j - 0.0f), Vector3(-1.0f, 0.0f, 0.0f)));
					GeomertyUtils::GenerateFaceIndicies(true, verticies.size(), indicies);
					break;
				case CUBE_RIGHT:
					verticies.push_back(Vertex3D(Vector3(1.0f, i - 0.0f, j + disp), Vector3(1.0f, 0.0f, 0.0f)));
					verticies.push_back(Vertex3D(Vector3(1.0f, i - 0.0f, j - 0.0f), Vector3(1.0f, 0.0f, 0.0f)));
					verticies.push_back(Vertex3D(Vector3(1.0f, i + disp, j - 0.0f), Vector3(1.0f, 0.0f, 0.0f)));
					verticies.push_back(Vertex3D(Vector3(1.0f, i + disp, j + disp), Vector3(1.0f, 0.0f, 0.0f)));
					GeomertyUtils::GenerateFaceIndicies(true, verticies.size(), indicies);
					break;
				}

			}
		}
}

void GeomertyUtils::GenerateFaceIndicies(bool clockwise, int size, std::vector<unsigned int>& indicies) {
	if (clockwise) {
		indicies.push_back(size - 4);
		indicies.push_back(size - 3);
		indicies.push_back(size - 2);

		indicies.push_back(size - 2);
		indicies.push_back(size - 1);
		indicies.push_back(size - 4);
	}
	else {
		indicies.push_back(size - 1);
		indicies.push_back(size - 2);
		indicies.push_back(size - 3);

		indicies.push_back(size - 3);
		indicies.push_back(size - 4);
		indicies.push_back(size - 1);
	}
}
