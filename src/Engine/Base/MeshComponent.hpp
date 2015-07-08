#pragma once

#include <Core/Common.hpp>
#include <ECS/Component.hpp>

#include <GL/glew.h>
#include <Graphics/Vertex3D.hpp>

struct MeshComponent : public Component {
	uint8* memory;
	Vertex3D* vertices;
	uint32* indices;
	uint32 indexCount;
	uint32 vertexCount;

	MeshComponent(uint32 vertexCount, uint32 indexCount) {
		uint32 vertexMemorySize = vertexCount * sizeof(Vertex3D);
		uint32 indexMemorySize = indexCount * sizeof(uint32);
		memory = new uint8[vertexMemorySize + indexMemorySize];
		vertices = (Vertex3D*)memory;
		indices = (uint32*)(memory + vertexMemorySize);
		this->vertexCount = vertexCount;
		this->indexCount = indexCount;
	}
	~MeshComponent() {
		delete[] memory;
	}

	void MeshComponent::CalculateNormals() {
		for (uint32 i = 0; i < vertexCount; i++) {
			vertices[i].normal = Vector3(0.0f, 0.0f, 0.0f);
		}
		for (uint32 i = 0; i < indexCount; i += 3) {
			uint32 index0 = indices[i + 0];
			uint32 index1 = indices[i + 1];
			uint32 index2 = indices[i + 2];
			Vector3 vert1 = vertices[index1].position - vertices[index0].position;
			Vector3 vert2 = vertices[index2].position - vertices[index0].position;

			Vector3 normal = vert1.Cross(vert2);
			normal.Normalize();
			vertices[index0].normal += normal;
			vertices[index1].normal += normal;
			vertices[index2].normal += normal;
		}
		for(uint32 i = 0; i < vertexCount; i++) {
			vertices[i].normal.Normalize();
		}
	}
};
