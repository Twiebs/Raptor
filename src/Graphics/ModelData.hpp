#pragma once
#include <Math/Vector3.hpp>
#include <Math/Vector2.hpp>
#include <GL/glew.h>

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


struct StaticMeshData {
	U32 vertexCount, indexCount;
	Vertex3D* vertices;
	U32* indices;
	U8* memblock;
};

struct Material {
	GLuint diffuseMapID = 0;
	GLuint specularMapID = 0;
	GLuint normalMapID = 0;
	~Material();
};

struct DebugModelData {
	std::vector<StaticMeshData> meshes;
	std::vector<U32> meshMaterialIndex;
	std::vector<VertexBufferGroup> meshVertexBuffers;
	std::vector<Material> materials;

	void ImportFromFile(const std::string& filename);
};



