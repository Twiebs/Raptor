#include "Geometry3D.hpp"
#include <Graphics/Render3D.hpp>

#include <unordered_map>

V3 Cuboid::Center() {
	V3 result = (size * 0.5f) + (position * 0.5f);
	return result;
}

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

class Vec3KeyFuncs {
public:
	size_t operator()(const V3& k)const {
		return std::hash<float>()(k.x) ^ std::hash<float>()(k.y) ^ std::hash<float>()(k.z);
	}

	bool operator()(const V3& a, const V3& b)const {
		return a.x == b.x && a.y == b.y && a.z == b.z;
	}
};

inline V3 findMidpoint(V3 vertex1, V3 vertex2) {
	return Normalize(V3((vertex1.x + vertex2.x) / 2.0f, (vertex1.y + vertex2.y) / 2.0f, (vertex1.z + vertex2.z) / 2.0f));
}

/// Generates an icosphere with radius 1.0f.
/// @param lod: Number of subdivisions
/// @param indices: Resulting indices for use with glDrawElements
/// @param positions: Resulting vertex positions
void generateIcosphereMesh(size_t lod, std::vector<uint32_t>& indices, std::vector<V3>& positions) {
	std::vector<uint32_t> newIndices;
	newIndices.reserve(256);

	std::unordered_map<V3, uint32_t, Vec3KeyFuncs, Vec3KeyFuncs> vertexLookup;

	indices.resize(ICOSOHEDRON_INDEX_COUNT);
	for (uint32_t i = 0; i < ICOSOHEDRON_INDEX_COUNT; i++) {
		indices[i] = ICOSOHEDRON_INDICES[i];
	}
	positions.resize(ICOSOHEDRON_VERTEX_COUNT);
	for (uint32_t i = 0; i < ICOSOHEDRON_VERTEX_COUNT; i++) {
		positions[i] = Normalize(ICOSOHEDRON_VERTICES[i]);
		vertexLookup[Normalize(ICOSOHEDRON_VERTICES[i])] = i;
	}

	for (size_t i = 0; i < (size_t)lod; i++) {
		for (size_t j = 0; j < indices.size(); j += 3) {
			/*
			j
			mp12   mp13
			j+1    mp23   j+2
			*/
			// Defined in counter clockwise order
			V3 vertex1 = positions[indices[j + 0]];
			V3 vertex2 = positions[indices[j + 1]];
			V3 vertex3 = positions[indices[j + 2]];

			V3 midPoint12 = findMidpoint(vertex1, vertex2);
			V3 midPoint23 = findMidpoint(vertex2, vertex3);
			V3 midPoint13 = findMidpoint(vertex1, vertex3);

			uint32_t mp12Index;
			uint32_t mp23Index;
			uint32_t mp13Index;

			auto iter = vertexLookup.find(midPoint12);
			if (iter != vertexLookup.end()) { // It is in the map
				mp12Index = iter->second;
			} else { // Not in the map
				mp12Index = (uint32_t)positions.size();
				positions.push_back(midPoint12);
				vertexLookup[midPoint12] = mp12Index;
			}

			iter = vertexLookup.find(midPoint23);
			if (iter != vertexLookup.end()) { // It is in the map
				mp23Index = iter->second;
			} else { // Not in the map
				mp23Index = (uint32_t)positions.size();
				positions.push_back(midPoint23);
				vertexLookup[midPoint23] = mp23Index;
			}

			iter = vertexLookup.find(midPoint13);
			if (iter != vertexLookup.end()) { // It is in the map
				mp13Index = iter->second;
			} else { // Not in the map
				mp13Index = (uint32_t)positions.size();
				positions.push_back(midPoint13);
				vertexLookup[midPoint13] = mp13Index;
			}

			newIndices.push_back(indices[j]);
			newIndices.push_back(mp12Index);
			newIndices.push_back(mp13Index);

			newIndices.push_back(mp12Index);
			newIndices.push_back(indices[j + 1]);
			newIndices.push_back(mp23Index);

			newIndices.push_back(mp13Index);
			newIndices.push_back(mp23Index);
			newIndices.push_back(indices[j + 2]);

			newIndices.push_back(mp12Index);
			newIndices.push_back(mp23Index);
			newIndices.push_back(mp13Index);
		}
		indices.swap(newIndices);
		newIndices.clear();
	}
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