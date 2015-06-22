#pragma once

#include <Core/Common.hpp>
#include <ECS/Component.hpp>

#include <GL/glew.h>
#include <Graphics/Vertex3D.hpp>

struct MeshComponent : public Component {
	Vertex3D* vertices;
	uint32* indices;
	uint8* memory;

	uint32 indexCount;
	uint32 vertexCount;

	GLuint vertexArrayID;
	GLuint vertexBufferID;
	GLuint elementBufferID;

	MeshComponent(uint32 vertexCount, uint32 indexCount) {
		uint32 vertexMemorySize = vertexCount * sizeof(Vertex3D);
		uint32 indexMemorySize = indexCount * sizeof(uint32);
		memory = new uint8[vertexMemorySize + indexMemorySize];
		vertices = (Vertex3D*)memory;
		indices = (uint32*)(memory + vertexMemorySize);
	}

	~MeshComponent() {
		delete[] memory;
	}
};
