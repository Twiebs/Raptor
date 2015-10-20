#include "Procedural3D.hpp"


// This is compleatly wrong... the vertex normals will not be summed together properly
// because the vertices are duplicated!
void CalculateSurfaceNormals (Vertex3D* vertices, U32 vertexCount, U32* indices, U32 indexCount) {
	assert(vertexCount == 65536 && indexCount == 98304);

	for (U32 i = 0; i < indexCount; i+=3) {
		auto index0 = indices[i + 0];
		auto index1 = indices[i + 1];
		auto index2 = indices[i + 2];

		auto edge0 = vertices[index1].position - vertices[index0].position;
		auto edge1 = vertices[index2].position - vertices[index0].position;
		auto normal = cross(edge0, edge1);

		vertices[index0].normal += normal;
		vertices[index1].normal += normal;
		vertices[index2].normal += normal;
	}

	for (U32 i = 0; i < vertexCount; i++) {
		vertices[i].normal.Normalize();
	}


}

//void CreatePlaneMesh (MeshData* meshData, float x, float y, float w, float h, int cellCount, int cellsPerUV, std::function<float(float,float)> heightFunction) {
//	auto cellWidth = w / cellCount;
//	auto cellHeight = h / cellCount;
//	auto uvStep = 1.0f / cellsPerUV;
//
//	auto gridArea = cellCount * cellCount;
//	auto vertexCount = gridArea * 4;
//	auto indexCount = gridArea * 6;
//	AllocateMeshData(meshData, vertexCount, indexCount);
//
//	auto vertices = meshData->vertices;
//	auto indices = meshData->indices;
//
//	U32 vertexIndex = 0;
//	for (U32 i = 0; i < gridArea; i++) {
//		assert(vertexIndex < vertexCount);
//		auto cellX = (i % cellCount);
//		auto cellY = i / cellCount;
//		auto vertexX = x + (cellX * cellWidth);
//		auto vertexY = y + (cellY * cellHeight);
//
//		auto uvXScalar = (cellX) % (cellsPerUV);
//		auto uvYScalar = (cellY) % (cellsPerUV);
//		auto setVertex = [&](Vertex3D* vertex, int xoffset, int yoffset) {
//			auto texCoordX = (uvXScalar * uvStep) + (uvStep * xoffset);
//			auto texCoordY = (uvYScalar * uvStep) + (uvStep * yoffset);
//			auto height = heightFunction == nullptr ? 0 : heightFunction(vertexX + (xoffset * cellWidth), vertexY + (yoffset * cellHeight));
//			vertex->position = Vector3(vertexX + (xoffset * cellWidth), height, vertexY + (yoffset * cellHeight));
//			vertex->normal = Vector3(0.0f, 0.0f, 0.0f);	// @INCOMPLETE Vertex normal not calculated
//			// vertex->normal = Vector3(0.0f, 1.0f, 0.0f);	// @INCOMPLETE Vertex normal not calculated
//			vertex->texCoord = Vector2(texCoordX, texCoordY);
//		};
//
//		setVertex(&vertices[vertexIndex++], 0, 0);
//		setVertex(&vertices[vertexIndex++], 1, 0);
//		setVertex(&vertices[vertexIndex++], 1, 1);
//		setVertex(&vertices[vertexIndex++], 0, 1);
//	}
//
//	vertexIndex = 0;
//	for (U32 i = 0; i < indexCount; i+=6) {
//		assert(vertexIndex < vertexCount);
//		indices[i + 0] = vertexIndex + 0;
//		indices[i + 1] = vertexIndex + 1;
//		indices[i + 2] = vertexIndex + 2;
//		indices[i + 3] = vertexIndex + 2;
//		indices[i + 4] = vertexIndex + 3;
//		indices[i + 5] = vertexIndex + 0;
//		vertexIndex += 4;
//	}
//
//
//	CalculateSurfaceNormals(vertices, vertexCount, indices, indexCount);
//}


void CreatePlaneMesh (MeshData* meshData, float x, float y, float w, float h, int cellCount, int cellsPerUV, std::function<float(float,float)> heightFunction) {
	auto cellWidth = w / cellCount;
	auto cellHeight = h / cellCount;
	auto uvStep = 1.0f / cellsPerUV;

	auto gridArea = cellCount * cellCount;
	auto vertexCount = gridArea * 4;
	auto indexCount = gridArea * 6;
	AllocateMeshData(meshData, vertexCount, indexCount);

	auto vertices = meshData->vertices;
	auto indices = meshData->indices;

	auto uvScalar = (1.0f / cellsPerUV);

	for (U32 i = 0; i < gridArea; i++) {
		assert(i < vertexCount);

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

	U32 currentIndex = 0;
	for (U32 i = 0; i < ((cellCount - 1) * (cellCount - 1)); i++) {
		auto cellX = (i % (cellCount - 1));
		auto cellY = (i / (cellCount - 1));

		indices[currentIndex + 0] = ((cellY + 0) * cellCount) + (cellX + 0);
		indices[currentIndex + 1] = ((cellY + 0) * cellCount) + (cellX + 1);
		indices[currentIndex + 2] = ((cellY + 1) * cellCount) + (cellX + 1);
		indices[currentIndex + 3] = ((cellY + 0) * cellCount) + (cellX + 0);
		indices[currentIndex + 4] = ((cellY + 1) * cellCount) + (cellX + 1);
		indices[currentIndex + 5] = ((cellY + 1) * cellCount) + (cellX + 0);
		currentIndex += 6;
	}


	CalculateSurfaceNormals(vertices, vertexCount, indices, indexCount);
}


