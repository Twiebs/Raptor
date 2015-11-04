#ifndef RAPTOR_MESH_HPP
#define RAPTOR_MESH_HPP

#include <Math/Vector2.hpp>
#include <Math/Vector3.hpp>
#include <GL/glew.h>

struct Vertex3D {
	Vector3 position;
	Vector3 normal;
	Vector2 texCoord;
	Vector3 tangent;
};

struct IndexedVertexBuffer {
	GLuint vertexArrayID = 0;
	GLuint vertexBufferID = 0;
	GLuint elementBufferID = 0;
};

struct MeshData {
	U32 vertexCount = 0, indexCount = 0;
	Vertex3D* vertices = nullptr;
	U32* indices = nullptr;
	U8* memblock = nullptr;
};

struct Mesh {
	U32 indexCount;
	GLuint vertexArrayID = 0;
	GLuint vertexBufferID = 0;
	GLuint elementBufferID = 0;
};

MeshData* AllocateMeshData (U32 vertexCount, U32 indexCount);

void AllocateMeshData (MeshData* meshData, U32 vertexCount, U32 indexCount);
void FreeMeshData (MeshData* meshData);

void InitIndexedVertexBuffer (IndexedVertexBuffer* buffer, MeshData* meshData);
void InitIndexedVertexBuffer (IndexedVertexBuffer* buffer, U32 vertexCount, U32 indexCount);
void FreeIndexedVertexBuffer (IndexedVertexBuffer* buffer);

void InitMesh (Mesh* mesh, MeshData* meshData);

#endif //RAPTOR_MESH_HPP
