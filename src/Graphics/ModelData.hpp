#pragma once
#include <Math/Vector3.hpp>
#include <Math/Vector2.hpp>

namespace Raptor {

struct VertexBufferGroup {
	U32 vertexArrayID = 0;
	U32 vertexBufferID = 0;
	U32 elementBufferID = 0;
	~VertexBufferGroup();
};

struct Vertex3D {
	Vector3 position;
	Vector3 normal;
	Vector2 texCoord;
	Vector3 tangent;
};

struct ImportedMeshData {
	U32 vertexCount, indexCount;
	Vertex3D* vertices;
	U32* indices;
	U8* memblock;
};

struct MeshData {
	U32 vertexCount, indexCount;
	Vertex3D* vertices;
	U32* indices;
};

struct MeshGPU {
	U32 vertexArrayID;
	U32 vertexBufferID;
	U32 elementBufferID;
};

struct MeshDrawable {
	U32 vertexArrayID;
	U32 indexCount;
};

struct Material {
	GLuint diffuseMapID = 0;
	GLuint specularMapID = 0;
	GLuint normalMapID = 0;
	~Material();
};

};
