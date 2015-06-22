#pragma once

#include <assert.h>

#include <Core/Common.hpp>
#include <Graphics/Vertex2D.hpp>

#include <GL/glew.h>

class DebugRenderer {
public:
      DebugRenderer(uint32 maxVertices =  65536);
      ~DebugRenderer();

      void Begin();
      void End();
      void Flush();

      void PushRect(float x, float y, float width, float height);

private:
      uint8* memory;
      Vertex2D* vertices;
      uint32* indices;

      GLuint vertexArrayID;
      GLuint vertexBufferID;
      GLuint elementBufferID;

      bool beginCalled;
      uint32 vertexIndex;
};
