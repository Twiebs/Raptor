#ifndef RAPTOR_MESH_HPP
#define RAPTOR_MESH_HPP

#include <Graphics/Render3D.hpp>

struct MeshVertexBuffer {
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


void AllocateMeshData (MeshData* meshData, U32 vertexCount, U32 indexCount);
void FreeMeshData (MeshData* meshData);
void InitMeshVertexBuffer (MeshVertexBuffer* buffer, MeshData* meshData);
void InitMesh (Mesh* mesh, MeshData* meshData);

#endif //RAPTOR_MESH_HPP
