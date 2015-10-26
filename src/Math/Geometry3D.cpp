#include "Geometry3D.hpp"

static const float GOLDEN_RATIO = 1.61803398875f;
static const int ICOSOHEDRON_VERTEX_COUNT = 12;
static const Vector3 ICOSOHEDRON_VERTICES[ICOSOHEDRON_VERTEX_COUNT] = {
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

static const int ICOSOHEDRON_INDEX_COUNT = 60;
static const U32 ICOSOHEDRON_INDICES[ICOSOHEDRON_INDEX_COUNT] = {
	0, 11, 5,
	0, 5, 1,
	0, 1, 7,
	0, 7, 10,
	0, 10, 11,

	1, 5, 9,
	5, 11, 4,
	11, 10, 2,
	10, 7, 6,
	7, 1, 8,

	3, 9, 4,
	3, 4, 2,
	3, 2, 6,
	3, 6, 8,
	3, 8, 9,

	4, 9, 5,
	2, 4, 11,
	6, 2, 10,
	8, 6, 7,
	9, 8, 1
};




void GenerateIcosphere() {

}




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