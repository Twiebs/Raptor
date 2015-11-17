#include "Procedural3D.hpp"

#include <Core/logging.h>

// TODO tangent vector calculation

void CalculateSurfaceNormals (Vertex3D* vertices, U32 vertexCount, U32* indices, U32 indexCount) {
	for (U32 i = 0; i < indexCount; i+=3) {
		auto index0 = indices[i + 0];
		auto index1 = indices[i + 1];
		auto index2 = indices[i + 2];

		auto edge0 = vertices[index1].position - vertices[index0].position;
		auto edge1 = vertices[index2].position - vertices[index0].position;
		auto normal = Cross(edge1, edge0);

		vertices[index0].normal += normal;
		vertices[index1].normal += normal;
		vertices[index2].normal += normal;
	}

	for (U32 i = 0; i < vertexCount; i++) {
		vertices[i].normal.Normalize();
	}
}

void GetPlaneMeshMemorySize (U32 meshResolution, U32* vertexCount, U32* indexCount) {
	auto gridArea = meshResolution * meshResolution;
	*vertexCount = (meshResolution + 1) * (meshResolution + 1);
	*indexCount = gridArea * 6;
}


void GeneratePlaneMesh (MeshData* meshData, float x, float y, float w, float h, U32 cellCount, float cellsPerUV, std::function<float(float, float)> heightFunction) {
#ifdef DEBUG
	assert(meshData->vertices != nullptr && "Mesh must already be initalized with memory!");
	assert(meshData->indices != nullptr);

	U32 vertexCount, indexCount;
	GetPlaneMeshMemorySize(cellCount, &vertexCount, &indexCount);
	assert(meshData->vertexCount == vertexCount);
	assert(meshData->indexCount == indexCount);
#endif

	auto cellWidth = w / cellCount;
	auto cellHeight = h / cellCount;

	U32 gridArea = cellCount * cellCount;

	
	auto vertices = meshData->vertices;
	auto indices = meshData->indices;

	auto uvScalar = (1.0f / cellsPerUV);
	if (heightFunction == nullptr) {
		for (U32 i = 0; i < meshData->vertexCount; i++) {
			assert(i < meshData->vertexCount);

			// Changing this to looping over x and y is probably faster
			// because no modulus is invloved.  The index can just be incremented
			// seperatly each iteration through the loop
			auto cellX = (i % (cellCount + 1));
			auto cellY = (i / (cellCount + 1)); // NOTE this calcuation will fail if not square
			auto vertexX = x + (cellX * cellWidth);
			auto vertexY = y + (cellY * cellHeight);
			float height = 0.0f;

			auto& vertex = vertices[i];
			vertex.position = Vector3(vertexX, height, vertexY);
			vertex.normal = Vector3(0.0f, 0.0f, 0.0f);
			vertex.texCoord = Vector2(cellX * uvScalar, cellY * uvScalar);
		}
	} else {
		for (U32 i = 0; i < gridArea; i++) {
			assert(i < meshData->vertexCount);

			auto cellX = (i % cellCount);
			auto cellY = i / cellCount; // NOTE this calcuation will fail if not square
			auto vertexX = x + (cellX * cellWidth);
			auto vertexY = y + (cellY * cellHeight);
			auto height = heightFunction(vertexX, vertexY);

			auto& vertex = vertices[i];
			vertex.position = Vector3(vertexX, height, vertexY);
			vertex.normal = Vector3(0.0f, 0.0f, 0.0f);
			vertex.texCoord = Vector2(cellX * uvScalar, cellY * uvScalar);
		}
	}

	U32 index = 0;
	for (U32 y = 0; y < cellCount; y++) {
		for (U32 x = 0; x < cellCount; x++) {
			indices[index++] = ((y + 0) * (cellCount + 1)) + (x + 0);
			indices[index++] = ((y + 0) * (cellCount + 1)) + (x + 1);
			indices[index++] = ((y + 1) * (cellCount + 1)) + (x + 1);
			indices[index++] = ((y + 0) * (cellCount + 1)) + (x + 0);
			indices[index++] = ((y + 1) * (cellCount + 1)) + (x + 1);
			indices[index++] = ((y + 1) * (cellCount + 1)) + (x + 0);
		}
	}

	assert(index == meshData->indexCount);
	CalculateSurfaceNormals(vertices, meshData->vertexCount, indices, meshData->indexCount);
}

//MeshData CreatePlaneMeshData (U32 cellCount) {
//	U32 vertexCount, indexCount;
//	GetPlaneMeshMemorySize(cellCount, &vertexCount, &indexCount);
//	return MeshData(vertexCount, indexCount);
//}


void CreatePlaneMesh (MeshData* meshData, float x, float y, float w, float h, U32 cellCount, float cellsPerUV, std::function<float(float,float)> heightFunction) {
	assert(meshData->vertices == nullptr && "Mesh data must be uninitalized when creating a plane mesh... Use Generate if you already have memory allocated!");
	U32 vertexCount, indexCount;
	GetPlaneMeshMemorySize(cellCount, &vertexCount, &indexCount);
	AllocateMeshData(meshData, vertexCount, indexCount);
	GeneratePlaneMesh(meshData, x, y, w, h, cellCount, cellsPerUV, heightFunction);
}


