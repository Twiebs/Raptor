#pragma once

#include <Graphics/Render3D.hpp>

namespace Raptor {

class DebugMesh {
public:
	GLuint vertexArrayID;
	Vertex3D* vertices;
	U32* indices;
	U32 vertexCount;
	U32 indexCount;

private:
	GLuint vertexBufferID;
	GLuint elementBufferID;
	U8* memory;

public:
	DebugMesh();
	DebugMesh(U32 vertexCount, U32 indexCount);
	~DebugMesh();

	void Init(U32 vertexCount, U32 indexCount);
	void InitGPU();
};

void Draw(const DebugMesh& mesh);

}