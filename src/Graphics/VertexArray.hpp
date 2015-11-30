#pragma once

#include <Core/types.h>
#include <GL/glew.h>

#include <Math/Vector2.hpp>
#include <Math/Vector3.hpp>

struct IndexedVertexBuffer {
	GLuint vertexArrayID = 0;
	GLuint vertexBufferID = 0;
	GLuint elementBufferID = 0;
};

template<typename TVertexType>
void CreateIndexedVertexArray (GLuint* vertexArray, GLuint* vertexBuffer, GLuint* indexBuffer,
	U32 vertexCount, U32 indexCount, GLenum drawMode, TVertexType* vertices = nullptr, U32* indices = nullptr);

template<typename TVertexType>
inline void CreateIndexedVertexArray (IndexedVertexBuffer* indexVertexBuffer, U32 vertexCount, U32 indexCount,
	GLenum drawMode, TVertexType* vertices = nullptr, U32* indices = nullptr);

template<typename TGenericVertex>
inline void SetVertexLayout() {
	static_assert(false, "There is no specialed template function for this type of vertex layout!");
}

void InitIndexedVertexBuffer (IndexedVertexBuffer* buffer, U32 vertexCount, U32 indexCount);
void FreeIndexedVertexBuffer (IndexedVertexBuffer* buffer);

template<typename TVertexType>
class DynamicIndexedVertexBuffer {
	GLuint vertexArrayID;
	GLuint vertexBufferID;
	GLuint elementBufferID;
	TVertexType* m_vertices;
	U32* m_indices;
	size_t currentVertexCount;
	size_t maxVertexCount;
	size_t currentIndexCount;
	size_t maxIndexCount;

public:
	DynamicIndexedVertexBuffer() { }
	DynamicIndexedVertexBuffer(U32 vertexCount, U32 indexCount, void(*vertexLayoutFN)(void)) { GenerateBuffer(vertexCount, indexCount, vertexLayoutFN); }
	~DynamicIndexedVertexBuffer();

	void GenerateBuffer(U32 vertexCount, U32 indexCount, void(*vertexLayoutFN)(void));

	//void PushVertices(const TVertexType* vertices, U32 count);
	//void PushIndices(const U32* indices, U32 count);
	void Push(const TVertexType* vertices, U32 vertexCount, const U32* indices, U32 indexCount);

	void Draw();
	void SendToGPU();
};



template<typename TVertexType>
void DynamicIndexedVertexBuffer<TVertexType>::GenerateBuffer(U32 vertexCount, U32 indexCount, void(*vertexLayoutFN)(void)) {
	/*CreateIndexedVertexArray(&vertexArrayID, &vertexBufferID, &elementBufferID,
		vertexCount, sizeof(TVertexType), indexCount, GL_DYNAMIC_DRAW, vertexLayoutFN);*/

	CreateIndexedVertexArray<V3> (
		&vertexArrayID, &vertexBufferID, &elementBufferID,
		vertexCount, indexCount, GL_DYNAMIC_DRAW
	);

	size_t verticesMemorySize = sizeof(TVertexType) * vertexCount;
	size_t indicesMemorySize = sizeof(U32) * indexCount;
	m_vertices = (TVertexType*)malloc(verticesMemorySize + indicesMemorySize);
	m_indices = (U32*)(m_vertices + vertexCount);

	currentIndexCount = 0;
	currentVertexCount = 0;
	maxIndexCount = indexCount;
	maxVertexCount = vertexCount;
}

template<typename TVertexType>
DynamicIndexedVertexBuffer<TVertexType>::~DynamicIndexedVertexBuffer() {
	glDeleteBuffers(1, &elementBufferID);
	glDeleteBuffers(1, &vertexBufferID);
	glDeleteVertexArrays(1, &vertexArrayID);
	free(m_vertices);
}

//template<typename TVertexType>
//void DynamicIndexedVertexBuffer<TVertexType>::PushVertices(const TVertexType* vertices, U32 count) {
//	assert((currentVertexCount + count) < maxVertexCount);
//	memcpy(&m_vertices[currentVertexCount], vertices, sizeof(TVertexType) * count);
//	currentVertexCount += count;
//}
//
//template<typename TVertexType>
//void DynamicIndexedVertexBuffer<TVertexType>::PushIndices(const U32* indices, U32 count) {
//	assert(currentIndexCount + count < maxIndexCount);
//	memcpy(&m_indices[currentIndexCount], indices, sizeof(U32) * count);
//	for (U32 i = 0; i < count; i++)
//		m_indices[currentIndexCount + i] += currentVertexCount;
//	currentIndexCount += count;
//}


template<typename TVertexType>
void DynamicIndexedVertexBuffer<TVertexType>::Push (const TVertexType* vertices, U32 vertexCount, const U32* indices, U32 indexCount) {
	assert((currentVertexCount + vertexCount) < maxVertexCount);
	memcpy(&m_vertices[currentVertexCount], vertices, sizeof(TVertexType) * vertexCount);

	assert(currentIndexCount + indexCount < maxIndexCount);
	memcpy(&m_indices[currentIndexCount], indices, sizeof(U32) * indexCount);
	for (U32 i = 0; i < indexCount; i++) {
		m_indices[currentIndexCount + i] += currentVertexCount;
	}

	currentVertexCount += vertexCount;
	currentIndexCount += indexCount;
}

template<typename TVertexType>
void DynamicIndexedVertexBuffer<TVertexType>::SendToGPU() {

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(TVertexType) * currentVertexCount, m_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//void* vertexGPUBuffer = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	//memcpy(vertexGPUBuffer, m_vertices, sizeof(TVertexType) * currentVertexCount);
	//glUnmapBuffer(GL_ARRAY_BUFFER);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferID);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(U32) * currentIndexCount, m_indices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//void* elementGPUBuffer = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
	//memcpy(elementGPUBuffer, m_indices, sizeof(U32) * currentIndexCount);
	//glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
}

template<typename TVertexType>
void DynamicIndexedVertexBuffer<TVertexType>::Draw() {
	SendToGPU();

#if 0
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	V3* vertices = (V3*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferID);
	U32* indices = (U32*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_READ_WRITE);
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
#endif

	glBindVertexArray(vertexArrayID);
	glDrawElements(GL_LINES, currentIndexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	currentVertexCount = 0;
	currentIndexCount = 0;
}

template<typename TVertex>
void CreateIndexedVertexArray (GLuint* vertexArray, GLuint* vertexBuffer, GLuint* indexBuffer,
	U32 vertexCount, U32 indexCount, GLenum drawMode, TVertex* vertices, U32* indices) 
{
	glGenVertexArrays(1, vertexArray);
	glBindVertexArray(*vertexArray);

	glGenBuffers(1, vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, *vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(TVertex), vertices, drawMode);

	glGenBuffers(1, indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(U32), indices, drawMode);

	SetVertexLayout<TVertex>();
	glBindVertexArray(0);
}

template<typename TVertexType>
inline void CreateIndexedVertexArray (IndexedVertexBuffer* buffer, U32 vertexCount, U32 indexCount,
	GLenum drawMode, TVertexType* vertices, U32* indices) 
{
	CreateIndexedVertexArray(&buffer->vertexArrayID, &buffer->vertexBufferID, &buffer->elementBufferID,
		vertexCount, indexCount, drawMode, vertices, indices);
}


struct Vertex3D {
	Vector3 position;
	Vector3 normal;
	Vector2 texCoord;
	Vector3 tangent;
};

template<>
inline void SetVertexLayout<V3>() {
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(V3), 0);
}

template<>
inline void SetVertexLayout<Vertex3D>() {
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)offsetof(Vertex3D, position));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)offsetof(Vertex3D, normal));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)offsetof(Vertex3D, texCoord));
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)offsetof(Vertex3D, tangent));
}
