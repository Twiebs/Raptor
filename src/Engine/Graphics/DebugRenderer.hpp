#pragma once

#include <assert.h>

#include <Core/Common.hpp>
#include <Graphics/GLSLProgram.hpp>
#include <Graphics/Color.hpp>
#include <Utils/IO.hpp>

#include <Math/Vector2.hpp>
#include <Math/Matrix4.hpp>

#include <GL/glew.h>

struct DebugVertex {
  Vector2 position;
};

const uint32 DEBUGIndices[] {
	0, 3, 2,
	0, 2, 1
};


class DebugRenderer {
public:
      DebugRenderer(uint32 maxVertices =  65536);
      ~DebugRenderer();

      void Begin();
      void End();
      void Flush();

      void PushRect(float x, float y, float width, float height);
      void PushCircle(float centerX, float centerY, float radius);

      void DEBUGBuildRect();
      void DEBUGDrawRect();
      void DEBUGDrawRect(float x, float y, float width, float height);

private:
      //The vertices and indices ptrs are offsets into the memoryblock
      DebugVertex* vertices;
      uint32* indices;
      uint8* memory;

      GLuint vertexArrayID = 0;
      GLuint vertexBufferID = 0;
      GLuint elementBufferID = 0;
      GLuint DEBUGVertexArrayID;
      GLuint DEBUGVertexBufferID;
      GLuint DEBUGElementBufferID;
      GLSLProgram* shader;

      bool beginCalled = false;
      uint32 vertexIndex = 0;

      std::string vertexShaderSource =
      "attribute vec4 position;"
      "void main() {"
        "gl_Position = position;"
      "}";

      std::string fragmentShaderSource =
      "void main() {"
        "gl_FragColor = vec4(1.0, 0.0, 1.0, 1.0);"
      "}";
};
