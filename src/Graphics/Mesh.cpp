#include "Mesh.hpp"

#include <Core/logging.h>



void InitMesh (Mesh* mesh, MeshData* meshData) {
	mesh->indexCount = meshData->indexCount;
	CreateIndexedVertexArray<Vertex3D>(&mesh->vertexArrayID, &mesh->vertexBufferID, &mesh->elementBufferID,
		meshData->vertexCount, meshData->indexCount, GL_STATIC_DRAW, meshData->vertices, meshData->indices);


	//glGenVertexArrays(1, &mesh->vertexArrayID);
	//glBindVertexArray(mesh->vertexArrayID);

	//glGenBuffers(1, &mesh->vertexBufferID);
	//glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBufferID);
	//glBufferData(GL_ARRAY_BUFFER, meshData->vertexCount * sizeof(Vertex3D), meshData->vertices, GL_STATIC_DRAW);

	//glGenBuffers(1, &mesh->elementBufferID);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->elementBufferID);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshData->indexCount * sizeof(U32), meshData->indices, GL_STATIC_DRAW);

	//glEnableVertexAttribArray(0);
	//glEnableVertexAttribArray(1);
	//glEnableVertexAttribArray(2);
	//glEnableVertexAttribArray(3);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)offsetof(Vertex3D, position));
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)offsetof(Vertex3D, normal));
	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)offsetof(Vertex3D, texCoord));
	//glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)offsetof(Vertex3D, tangent));
	//glBindVertexArray(0);
}

//MeshData::MeshData() : vertexCount(0), indexCount(0) { }
//
//MeshData::MeshData(U32 vertexCount, U32 indexCount)
//	: vertexCount(vertexCount), indexCount(indexCount) 
//{ 
//	size_t vertexMemorySize = sizeof(Vertex3D) * vertexCount;
//	size_t indexMemorySize = sizeof(U32) * indexCount;
//	vertices = (Vertex3D*)malloc(vertexMemorySize + indexMemorySize);
//	indices = (U32*)(vertices + vertexCount);
//}
//
//MeshData::~MeshData() {
//	free(vertices);
//}
//
//MeshData::MeshData(const MeshData& data) :
//	vertexCount(data.vertexCount), indexCount(data.indexCount), 
//	vertices(data.vertices), indices(data.indices) { }


void AllocateMeshData (MeshData* meshData, U32 vertexCount, U32 indexCount) {
	assert(meshData->vertices == nullptr && "Mesh data was already initalized!");
	meshData->vertexCount = vertexCount;
	meshData->indexCount = indexCount;
	size_t vertexMemorySize = sizeof(Vertex3D) * vertexCount;
	size_t indexMemorySize = sizeof(U32) * indexCount;
	meshData->vertices = (Vertex3D*)malloc(vertexMemorySize + indexMemorySize);
	meshData->indices = (U32*)(meshData->vertices + vertexCount);
}


void FreeMeshData (MeshData* meshData) {
	assert(meshData->vertices != nullptr);
	free(meshData->vertices);
	meshData->vertices = nullptr;
	meshData->indices = nullptr;
	meshData->indexCount = 0;
	meshData->vertexCount = 0;
}

//void InitIndexedVertexBuffer (IndexedVertexBuffer* buffer, MeshData* meshData) {
//	glGenVertexArrays(1, &buffer->vertexArrayID);
//	glBindVertexArray(buffer->vertexArrayID);
//
//	glGenBuffers(1, &buffer->vertexBufferID);
//	glBindBuffer(GL_ARRAY_BUFFER, buffer->vertexBufferID);
//	glBufferData(GL_ARRAY_BUFFER, meshData->vertexCount * sizeof(Vertex3D), meshData->vertices, GL_STATIC_DRAW);
//
//	glGenBuffers(1, &buffer->elementBufferID);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->elementBufferID);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshData->indexCount * sizeof(U32), meshData->indices, GL_STATIC_DRAW);
//
//	glEnableVertexAttribArray(0);
//	glEnableVertexAttribArray(1);
//	glEnableVertexAttribArray(2);
//	glEnableVertexAttribArray(3);
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)offsetof(Vertex3D, position));
//	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)offsetof(Vertex3D, normal));
//	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)offsetof(Vertex3D, texCoord));
//	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)offsetof(Vertex3D, tangent));
//	glBindVertexArray(0);
//}