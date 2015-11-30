#include "VertexArray.hpp"

#include <Math/Vector3.hpp>

//
//void InitIndexedVertexBuffer (IndexedVertexBuffer* buffer, U32 vertexCount, U32 indexCount) {
//	glGenVertexArrays(1, &buffer->vertexArrayID);
//	glBindVertexArray(buffer->vertexArrayID);
//
//	glGenBuffers(1, &buffer->vertexBufferID);
//	glBindBuffer(GL_ARRAY_BUFFER, buffer->vertexBufferID);
//	glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(Vertex3D), nullptr, GL_STATIC_DRAW);
//
//	glGenBuffers(1, &buffer->elementBufferID);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->elementBufferID);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(U32), nullptr, GL_STATIC_DRAW);
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
//
//
//void Init1PIndexedVertexBuffer(IndexedVertexBuffer* buffer, U32 vertexCount, U32 indexCount, GLenum drawMode) {
//	glGenVertexArrays(1, &buffer->vertexArrayID);
//	glBindVertexArray(buffer->vertexArrayID);
//
//	glGenBuffers(1, &buffer->vertexBufferID);
//	glBindBuffer(GL_ARRAY_BUFFER, buffer->vertexBufferID);
//	glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(V3), nullptr, drawMode);
//
//	glGenBuffers(1, &buffer->elementBufferID);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->elementBufferID);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(U32), nullptr, drawMode);
//
//	glEnableVertexAttribArray(0);
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(V3), 0);
//	glBindVertexArray(0);
//}


void FreeIndexedVertexBuffer (IndexedVertexBuffer* buffer) {
	glDeleteVertexArrays(1, &buffer->vertexArrayID);
	glDeleteBuffers(1, &buffer->vertexBufferID);
	glDeleteBuffers(1, &buffer->elementBufferID);
}
