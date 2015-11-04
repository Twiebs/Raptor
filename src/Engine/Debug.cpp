#include "Debug.hpp"

#include <Graphics/imgui.h>

using namespace Raptor;

#include <Graphics/Mesh.hpp>
#include <Graphics/Model.hpp>
#include <Graphics/ModelData.hpp>

namespace Raptor {
void DebugMesh::InitGPU() {
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	glGenBuffers(1, &vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(Vertex3D), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &elementBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(U32), indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)offsetof(Vertex3D, position));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)offsetof(Vertex3D, normal));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)offsetof(Vertex3D, texCoord));
	glBindVertexArray(0);
}

DebugMesh::~DebugMesh() {
	if (memory != nullptr) {
		delete[] memory;
	}
	if (vertexArrayID != 0) {
		glDeleteVertexArrays(1, &vertexArrayID);
		glDeleteBuffers(1, &vertexBufferID);
		glDeleteBuffers(1, &elementBufferID);
	}
}


DebugMesh::DebugMesh() {
	vertexCount = 0;
}

DebugMesh::DebugMesh(U32 vertexCount, U32 indexCount) {
	Init(vertexCount, indexCount);
}

void DebugMesh::Init(U32 vertexCount, U32 indexCount) {
	this->vertexCount = vertexCount;
	this->indexCount = indexCount;
	auto verticesMemorySize = sizeof(Vertex3D) * vertexCount;
	auto indiciesMemorySize = sizeof(U32) * indexCount;
	memory = new U8[verticesMemorySize + indiciesMemorySize];
	vertices = (Vertex3D*)memory;
	indices = (U32*)(memory + verticesMemorySize);
}


void Draw(const DebugMesh& mesh) {
	glBindVertexArray(mesh.vertexArrayID);
	glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, 0);
}

}