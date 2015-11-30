#ifndef RAPTOR_MESH_HPP
#define RAPTOR_MESH_HPP

#include <GL/glew.h>

#include <Math/Vector2.hpp>
#include <Math/Vector3.hpp>
#include <Core/logging.h>

#include <Graphics/Vertex3D.hpp>
#include <Graphics/VertexArray.hpp>

struct MeshData {
	U32 vertexCount = 0, indexCount = 0;
	Vertex3D* vertices = nullptr;
	U32* indices = nullptr;
};


struct Mesh {
	U32 indexCount;
	GLuint vertexArrayID = 0;
	GLuint vertexBufferID = 0;
	GLuint elementBufferID = 0;
};

void InitIndexedVertexBuffer (IndexedVertexBuffer* buffer, MeshData* meshData);

void AllocateMeshData (MeshData* meshData, U32 vertexCount, U32 indexCount);
void FreeMeshData (MeshData* meshData);
void InitMesh (Mesh* mesh, MeshData* meshData);

#endif //RAPTOR_MESH_HPP
