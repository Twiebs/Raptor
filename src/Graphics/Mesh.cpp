#include "Mesh.hpp"

#include <Core/logging.h>

void InitMesh (Mesh* mesh, MeshData* meshData) {
	mesh->indexCount = meshData->indexCount;
	glGenVertexArrays(1, &mesh->vertexArrayID);
	glBindVertexArray(mesh->vertexArrayID);

	glGenBuffers(1, &mesh->vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, meshData->vertexCount * sizeof(Vertex3D), meshData->vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &mesh->elementBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->elementBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshData->indexCount * sizeof(U32), meshData->indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)offsetof(Vertex3D, position));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)offsetof(Vertex3D, normal));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)offsetof(Vertex3D, texCoord));
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)offsetof(Vertex3D, tangent));
	glBindVertexArray(0);
}

MeshData* AllocateMeshData (U32 vertexCount, U32 indexCount) {
	size_t vertexMemorySize = sizeof(Vertex3D) * vertexCount;
	size_t indexMemorySize = sizeof(U32) * indexCount;
	MeshData* data = (MeshData*)malloc(vertexMemorySize + indexMemorySize + sizeof(MeshData));
	data->vertexCount = vertexCount;
	data->indexCount = indexCount;
	data->vertices = (Vertex3D*)(data + 1);
	data->indices = (U32*)(data->vertices + vertexCount);
	return data;
}

void AllocateMeshData (MeshData* meshData, U32 vertexCount, U32 indexCount) {
	assert(meshData->memblock == nullptr && "Mesh data was already initalized!");
	meshData->vertexCount = vertexCount;
	meshData->indexCount = indexCount;
	size_t vertexMemorySize = sizeof(Vertex3D) * vertexCount;
	size_t indexMemorySize = sizeof(U32) * indexCount;
	meshData->memblock = (U8*)malloc(vertexMemorySize + indexMemorySize);
	meshData->vertices = (Vertex3D*)meshData->memblock;
	meshData->indices = (U32*)(meshData->memblock + vertexMemorySize);
}


void FreeMeshData (MeshData* meshData) {
	assert(meshData->memblock != nullptr);
	free(meshData->memblock);
	meshData->memblock = nullptr;
	meshData->vertices = nullptr;
	meshData->indices = nullptr;
	meshData->indexCount = 0;
	meshData->vertexCount = 0;
}

void InitIndexedVertexBuffer (IndexedVertexBuffer* buffer, MeshData* meshData) {
	glGenVertexArrays(1, &buffer->vertexArrayID);
	glBindVertexArray(buffer->vertexArrayID);

	glGenBuffers(1, &buffer->vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, buffer->vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, meshData->vertexCount * sizeof(Vertex3D), meshData->vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &buffer->elementBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->elementBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshData->indexCount * sizeof(U32), meshData->indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)offsetof(Vertex3D, position));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)offsetof(Vertex3D, normal));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)offsetof(Vertex3D, texCoord));
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)offsetof(Vertex3D, tangent));
	glBindVertexArray(0);
}

void InitIndexedVertexBuffer (IndexedVertexBuffer* buffer, U32 vertexCount, U32 indexCount) {
	glGenVertexArrays(1, &buffer->vertexArrayID);
	glBindVertexArray(buffer->vertexArrayID);

	glGenBuffers(1, &buffer->vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, buffer->vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(Vertex3D), nullptr, GL_STATIC_DRAW);

	glGenBuffers(1, &buffer->elementBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->elementBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(U32), nullptr, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)offsetof(Vertex3D, position));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)offsetof(Vertex3D, normal));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)offsetof(Vertex3D, texCoord));
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)offsetof(Vertex3D, tangent));
	glBindVertexArray(0);
}

void FreeIndexedVertexBuffer (IndexedVertexBuffer* buffer) {
	glDeleteVertexArrays(1, &buffer->vertexArrayID);
	glDeleteBuffers(1, &buffer->vertexBufferID);
	glDeleteBuffers(1, &buffer->elementBufferID);
}