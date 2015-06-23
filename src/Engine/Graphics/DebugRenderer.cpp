#include "DebugRenderer.hpp"

DebugRenderer::DebugRenderer(uint32 maxVertices) {
      //Allocate a buffer to write our verticies and incides into
      uint32 indexCount = maxVertices * 6;
      uint32 verticesMemorySize = sizeof(DebugVertex) * maxVertices;
      uint32 indicesMemorySize = indexCount * sizeof(uint32);
      memory = new uint8[verticesMemorySize + indicesMemorySize];
      vertices = (DebugVertex*)memory;
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

      Matrix4 mvp = Matrix4::Ortho(0.0f, 1280.0f, 0.0f, 720.0f, 0.1f, 1000.0f, 1.0f);
      shader = DEBUGLoadShaderFromSource(vertexShaderSource, fragmentShaderSource);
      shader->Use();
      GLuint mvpLoc = glGetUniformLocation(shader->GetProgram(), "mvp");
      glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &mvp[0][0]);

      glGenVertexArrays(1, &vertexArrayID);
      glBindVertexArray(vertexArrayID);

      glGenBuffers(1, &vertexBufferID);
      glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
      glBufferData(GL_ARRAY_BUFFER, verticesMemorySize, NULL, GL_DYNAMIC_DRAW);	//No data is actualy buffered into the array...

      glGenBuffers(1, &elementBufferID);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferID);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesMemorySize, indices, GL_DYNAMIC_DRAW);

      GLuint positionLocation = 0;
      GLuint colorLocation = 1;
#ifdef OPENGL_ES
      positionLocation = glGetAttribLocation(shader->GetProgram(), "position");
      colorLocation = glGetAttribLocation(shader->GetProgram(), "color");
#endif

      glEnableVertexAttribArray(positionLocation);
      glVertexAttribPointer(positionLocation, 2, GL_FLOAT, GL_FALSE, sizeof(DebugVertex), (GLvoid*)offsetof(DebugVertex, position));
      glEnableVertexAttribArray(colorLocation);
      glVertexAttribPointer(colorLocation, 4, GL_FLOAT, GL_FALSE, sizeof(DebugVertex), (GLvoid*)offsetof(DebugVertex, color));

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
      Color color(0.0f, 1.0f, 1.0f, 1.0f);
      vertices[vertexIndex + 0] = DebugVertex {Vector2(x, y), color};
      vertices[vertexIndex + 1] = DebugVertex { Vector2(x + width, y), color};
      vertices[vertexIndex + 2] = DebugVertex { Vector2(x + width, y + height), color};
      vertices[vertexIndex + 3] = DebugVertex { Vector2(x, y + height), color};
      vertexIndex += 4;
}

void DebugRenderer::Begin() {
      assert(!beginCalled);
	beginCalled = true;
}

void DebugRenderer::End() {
 	assert(beginCalled);
	beginCalled = false;
	Flush();

	glBindVertexArray(debugVertexArrayID);
	glBindBuffer(GL_ARRAY_BUFFER, debugVertexBufferID);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void DebugRenderer::Flush() {
	shader->Use();
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(DebugVertex) * vertexIndex, (GLvoid*)vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(vertexArrayID);
	glDrawElements(GL_TRIANGLES, vertexIndex * 6, GL_UNSIGNED_INT, (GLvoid*)indices);
	glBindVertexArray(0);
	vertexIndex = 0;
}

void DebugRenderer::DEBUGBuildRect() {
	static float debugVertices[] {
		-0.5f, -0.5f,
		-0.5f, 0.5f,
		0.5f, 0.5f,

		0.5f, 0.5f,
		-0.5f, 0.5f,
		-0.5f, -0.5f
	};

	glGenVertexArrays(1, &debugVertexArrayID);
	glBindVertexArray(debugVertexArrayID);

	glGenBuffers(1, &debugVertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, debugVertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6, &debugVertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float), 0);
}
