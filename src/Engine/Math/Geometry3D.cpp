#include "Geometry3D.hpp"

Vector3 PointToQuadrilateralizedSphere(const Vector3& point) {
	float x2 = point.x * point.x;
	float y2 = point.y * point.y;
	float z2 = point.z * point.z;

	float dx = point.x * sqrtf((1.0f) - (y2 / 2.0f) - (z2 / 2.0f) + ((y2 * z2) / 3.0f));
	float dy = point.y * sqrtf((1.0f) - (z2 / 2.0f) - (x2 / 2.0f) + ((z2 * x2) / 3.0f));
	float dz = point.z * sqrtf((1.0f) - (x2 / 2.0f) - (y2 / 2.0f) + ((x2 * y2) / 3.0f));

	return Vector3(dx, dy, dz);
}

const static float GOLDEN_RATIO = 1.61803398875f;

const static int NUM_ICOSOHEDRON_VERTICES = 12;
const static Vector3 ICOSOHEDRON_VERTICES[12] = {
	Vector3(-1.0f, GOLDEN_RATIO, 0.0f),
	Vector3(1.0f, GOLDEN_RATIO, 0.0f),
	Vector3(-1.0f, -GOLDEN_RATIO, 0.0f),
	Vector3(1.0f, -GOLDEN_RATIO, 0.0f),

	Vector3(0.0f, -1.0f, GOLDEN_RATIO),
	Vector3(0.0f, 1.0f, GOLDEN_RATIO),
	Vector3(0.0f, -1.0, -GOLDEN_RATIO),
	Vector3(0.0f, 1.0f, -GOLDEN_RATIO),

	Vector3(GOLDEN_RATIO, 0.0f, -1.0f),
	Vector3(GOLDEN_RATIO, 0.0f, 1.0f),
	Vector3(-GOLDEN_RATIO, 0.0f, -1.0f),
	Vector3(-GOLDEN_RATIO, 0.0, 1.0f)
};

void BuildCube(int subdivisionLevel, Vector3* positions, U32* indices) {
	U32 postionIndex = 0, indiceIndex = 0;
	auto verticesPerQuad = GetVertexCountForQuad(subdivisionLevel);
	auto indicesPerQuad = GetIndexCountForQuad(subdivisionLevel);
	BuildFrontQuad(subdivisionLevel, positions + postionIndex, indices + indiceIndex);
	postionIndex += verticesPerQuad;
	indiceIndex += indicesPerQuad;
	//etc..
}

U32 GetVertexCountForQuad(int subdivisionLevel) {
	return (subdivisionLevel * subdivisionLevel) * 4;
}
U32 GetIndexCountForQuad(int subdivisionLevel) {
	return (subdivisionLevel * subdivisionLevel) * 6;
}

void BuildFrontQuad(int subdivisionLevel, Vector3* positions, U32* indices) {
	U32 posIdx = 0, indiceIdx = 0;
	auto step = 1.0f / (float)subdivisionLevel;
	for (auto i = 0; i < subdivisionLevel; i++) {
		for (auto j = 0; j < subdivisionLevel; j++) {
			float x = ((float)i / (float)subdivisionLevel);
			float y = ((float)j / (float)subdivisionLevel);
			positions[posIdx + 0] = Vector3(x, y, 1.0f);
			positions[posIdx + 1] = Vector3(x + step, y, 1.0f);
			positions[posIdx + 2] = Vector3(x + step, y + step, 1.0f);
			positions[posIdx + 3] = Vector3(x, y + step, 1.0f);
			indices[indiceIdx + 0] = posIdx + 0;
			indices[indiceIdx + 1] = posIdx + 3;
			indices[indiceIdx + 2] = posIdx + 2;
			indices[indiceIdx + 3] = posIdx + 0;
			indices[indiceIdx + 4] = posIdx + 2;
			indices[indiceIdx + 5] = posIdx + 1;
			posIdx += 4;
			indiceIdx += 6;
		}
	}
}