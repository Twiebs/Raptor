#ifndef RAPTOR_MESH_HPP
#define RAPTOR_MESH_HPP

#include <GL/glew.h>

#include <Math/Vector2.hpp>
#include <Math/Vector3.hpp>
#include <Core/logging.h>

void CreateIndexedVertexArray (GLuint* vertexArray, GLuint* vertexBuffer, GLuint* indexBuffer,
	U32 vertexCount, size_t vertexMemorySize, U32 indexCount, GLenum drawMode, 
	void(*vertexLayoutFn)(void), void* vertices = nullptr, void* indices = nullptr);

void SetVertexLayout1P();


struct Vertex3D {
	Vector3 position;
	Vector3 normal;
	Vector2 texCoord;
	Vector3 tangent;
};


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
	
	void PushVertices(const TVertexType* vertices, U32 count);
	void PushIndices(const U32* indices, U32 count);
	void Push(const TVertexType* vertices, U32 vertexCount, const U32* indices, U32 indexCount);

	void Draw();
	void SendToGPU();
};

template<typename TVertexType>
void DynamicIndexedVertexBuffer<TVertexType>::GenerateBuffer(U32 vertexCount, U32 indexCount, void(*vertexLayoutFN)(void)) {
	CreateIndexedVertexArray(&vertexArrayID, &vertexBufferID, &elementBufferID, 
		vertexCount, sizeof(TVertexType), indexCount, GL_DYNAMIC_DRAW, vertexLayoutFN);

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

template<typename TVertexType>
void DynamicIndexedVertexBuffer<TVertexType>::PushVertices(const TVertexType* vertices, U32 count) {
	assert((currentVertexCount + count) < maxVertexCount);
	memcpy(&m_vertices[currentVertexCount], vertices, sizeof(TVertexType) * count);
	currentVertexCount += count;
}

template<typename TVertexType>
void DynamicIndexedVertexBuffer<TVertexType>::PushIndices(const U32* indices, U32 count) {
	assert(currentIndexCount + count < maxIndexCount);
	memcpy(&m_indices[currentIndexCount], indices, sizeof(U32) * count);
	currentIndexCount += count;
}

template<typename TVertexType>
void DynamicIndexedVertexBuffer<TVertexType>::Push (const TVertexType* vertices, U32 vertexCount, const U32* indices, U32 indexCount) {
	PushVertices(vertices, vertexCount);
	PushIndices(indices, indexCount);
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

#if 1
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	V3* vertices = (V3*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferID);
	U32* indices = (U32*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_READ_WRITE);
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
#endif
	
	glBindVertexArray(vertexArrayID);
	glDrawElements(GL_TRIANGLES, currentIndexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	currentVertexCount = 0;
	currentIndexCount = 0;
}

struct IndexedVertexBuffer {
	GLuint vertexArrayID = 0;
	GLuint vertexBufferID = 0;
	GLuint elementBufferID = 0;
};


struct MeshData {
	U32 vertexCount = 0, indexCount = 0;
	Vertex3D* vertices = nullptr;
	U32* indices = nullptr;
};

//struct MeshData {
//	const U32 vertexCount;
//	const U32 indexCount;
//
//	Vertex3D* vertices = nullptr;
//	U32* indices = nullptr;
//
//	MeshData(U32 vertexCount, U32 indexCount);
//	MeshData(const MeshData& data);
//	MeshData();
//	~MeshData();
//};

struct Mesh {
	U32 indexCount;
	GLuint vertexArrayID = 0;
	GLuint vertexBufferID = 0;
	GLuint elementBufferID = 0;
};


void AllocateMeshData (MeshData* meshData, U32 vertexCount, U32 indexCount);
void FreeMeshData (MeshData* meshData);

void InitIndexedVertexBuffer (IndexedVertexBuffer* buffer, MeshData* meshData);
void InitIndexedVertexBuffer (IndexedVertexBuffer* buffer, U32 vertexCount, U32 indexCount);
void FreeIndexedVertexBuffer (IndexedVertexBuffer* buffer);

void InitMesh (Mesh* mesh, MeshData* meshData);

#endif //RAPTOR_MESH_HPP
