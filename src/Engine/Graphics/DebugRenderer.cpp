#include "DebugRenderer.hpp"

DebugRenderer::DebugRenderer(uint32 maxVertices) {
	//Allocate a buffer to write our vertices and indices into
	uint32 indexCount = (maxVertices / 4) * 6;
	uint32 verticesMemorySize = sizeof(DebugVertex) * maxVertices;
	uint32 indicesMemorySize = indexCount * sizeof(uint32);
	memory = new uint8[verticesMemorySize + indicesMemorySize];
	vertices = (DebugVertex*) memory;
	indices = (uint32*) (memory + verticesMemorySize);

	//Set the array of indices since they never change
	for (uint32 i = 0, vertexIdx = 0; i < indexCount; i += 6, vertexIdx += 4) {
		indices[i + 0] = vertexIdx + 0;
		indices[i + 1] = vertexIdx + 3;
		indices[i + 2] = vertexIdx + 2;

		indices[i + 3] = vertexIdx + 0;
		indices[i + 4] = vertexIdx + 2;
		indices[i + 5] = vertexIdx + 1;
	}

	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	glGenBuffers(1, &vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, verticesMemorySize, NULL, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &elementBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesMemorySize, indices,GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	DEBUGBuildRect();

	shader = DEBUGLoadShaderFromSource(vertexShaderSource, fragmentShaderSource);
}

DebugRenderer::~DebugRenderer() {
	delete[] memory;
	glDeleteVertexArrays(1, &vertexArrayID);
	glDeleteBuffers(1, &vertexBufferID);
	glDeleteBuffers(1, &elementBufferID);
}

//TODO consider just adding everything to the GLbuffer automaticly
//This is completely possible because there would be no need to texture swap / etc.
void DebugRenderer::PushRect(float x, float y, float width, float height) {
	DebugVertex verts[4] {
		DebugVertex { Vector2(x, y) },
		DebugVertex { Vector2(x + width, y) },
		DebugVertex { Vector2(x + width, y + height) },
		DebugVertex { Vector2(x, y + height) }
	};

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(DebugVertex) * vertexIndex, sizeof(float) * 2 * 4, verts);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	vertexIndex += 4;

//	Color color(0.0f, 1.0f, 1.0f, 1.0f);
//	vertices[vertexIndex + 0] = DebugVertex { Vector2(x, y) };
//	vertices[vertexIndex + 1] = DebugVertex { Vector2(x + width, y) };
//	vertices[vertexIndex + 2] = DebugVertex { Vector2(x + width, y + height) };
//	vertices[vertexIndex + 3] = DebugVertex { Vector2(x, y + height) };
}

void DebugRenderer::Begin() {
	assert(!beginCalled);
	beginCalled = true;
}

void DebugRenderer::End() {
	assert(beginCalled);
	beginCalled = false;
	Flush();
}

void DebugRenderer::Flush() {
	shader->Use();
//	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
//	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(DebugVertex) * vertexIndex, (GLvoid*) vertices);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(vertexArrayID);
	glDrawElements(GL_TRIANGLES, (vertexIndex / 4) * 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	vertexIndex = 0;
}

void DebugRenderer::DEBUGDrawRect() {
	glBindBuffer(GL_ARRAY_BUFFER, DEBUGVertexBufferID);
	glVertexAttribPointer(0 /* ? */, 2, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(0);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void DebugRenderer::DEBUGDrawRect(float x, float y, float width, float height) {
//	float debugVertices[] {
//			x, y,
//			x + width, y,
//			x + width, y + height,
//			x, y + height
//	};

	glBindVertexArray(DEBUGVertexArrayID);
	glDrawElements(GL_TRIANGLES, 6, GL_FLOAT, 0);
	glBindVertexArray(0);
}

void DebugRenderer::DEBUGBuildRect() {
	static float DEBUGVertices[] { -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, };

	glGenVertexArrays(1, &DEBUGVertexArrayID);
	glBindVertexArray(DEBUGVertexArrayID);

	glGenBuffers(1, &DEBUGVertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, DEBUGVertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 2, DEBUGVertices, GL_STATIC_DRAW);

	glGenBuffers(1, &DEBUGElementBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, DEBUGElementBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32) * 6, DEBUGIndices, GL_STATIC_DRAW),

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, 0, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
