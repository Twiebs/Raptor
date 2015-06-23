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
  Color color;
};

class DebugRenderer {
public:
      DebugRenderer(uint32 maxVertices =  65536);
      ~DebugRenderer();

      void Begin();
      void End();
      void Flush();

      void PushRect(float x, float y, float width, float height);

      void DEBUGBuildRect();

private:
      uint8* memory;
      DebugVertex* vertices;
      uint32* indices;

      GLuint vertexArrayID = 0;
      GLuint vertexBufferID = 0;
      GLuint elementBufferID = 0;
      GLSLProgram* shader;

      GLuint debugVertexArrayID;
      GLuint debugVertexBufferID;

      bool beginCalled = false;
      uint32 vertexIndex = 0;

      const std::string vertexShaderSource =
      "#version 100\n"
      "attribute vec2 position;"
      "void main() {"
        "gl_Position = vec4(position.xy, 0.0, 1.0);"
      "}";

      const std::string fragmentShaderSource =
      "#version 100\n"
      "void main() {"
        "gl_FragColor = vec4(1.0);"
      "}";
};
