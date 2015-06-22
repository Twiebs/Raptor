
#include "DebugRenderer.hpp"

DebugRenderer::DebugRenderer(uint32 maxVertices) {
      //Allocate a buffer to write our verticies and incides into
      uint32 indexCount = maxVertices * 6;
      uint32 verticesMemorySize = sizeof(Vertex2D) * maxVertices;
      uint32 indicesMemorySize = indexCount * sizeof(uint32);
      memory = new uint8[verticesMemorySize + indicesMemorySize];
      vertices = (Vertex2D*)memory;
      indices = (uint32*)(memory + verticesMemorySize);

      //Set the array of indices since they never change
      for(uint32 i = 0, vertexIdx = 0; i < indexCount; i += 6, vertexIdx += 4) {
            indices[i + 0] = vertexIdx + 0;
            indices[i + 1] = vertexIdx + 1;
            indices[i + 2] = vertexIdx + 2;

            indices[i + 3] = vertexIdx + 0;
            indices[i + 4] = vertexIdx + 2;
            indices[i + 5] = vertexIdx + 3;
      }

      glGenVertexArrays(1, &vertexArrayID);
      glBindVertexArray(vertexArrayID);

      glGenBuffers(1, &vertexBufferID);
      glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
      glBufferData(GL_ARRAY_BUFFER, verticesMemorySize, NULL, GL_DYNAMIC_DRAW);	//No data is actualy buffered into the array...

      glGenBuffers(1, &elementBufferID);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferID);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesMemorySize, &indices[0], GL_DYNAMIC_DRAW);

      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (GLvoid*)offsetof(Vertex2D, position));
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (GLvoid*)offsetof(Vertex2D, uv));
      glEnableVertexAttribArray(2);
      glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (GLvoid*)offsetof(Vertex2D, color));

      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glBindVertexArray(0);
}

DebugRenderer::~DebugRenderer() {
      delete[] memory;
      glDeleteVertexArrays(1, &vertexArrayID);
	glDeleteBuffers(1, &vertexBufferID);
	glDeleteBuffers(1, &elementBufferID);
}

void DebugRenderer::PushRect(float x, float y, float width, float height) {
      Color color;
      vertices[vertexIndex + 0] = Vertex2D {Vector2(x, y), Vector2(0.0f, 0.0f), color};
      vertices[vertexIndex + 1] = Vertex2D {Vector2(x + width, y), Vector2(1.0f, 0.0f), color};
      vertices[vertexIndex + 2] = Vertex2D {Vector2(x + width, y + height), Vector2(1.0f, 1.0f), color};
      vertices[vertexIndex + 3] = Vertex2D {Vector2(x, y + height), Vector2(0.0f, 1.0f), color};
}

void DebugRenderer::Begin() {
      assert(!beginCalled);
}

void DebugRenderer::End() {
      assert(beginCalled);
      Flush();
}

void DebugRenderer::Flush() {
      vertexIndex = 0;
}
