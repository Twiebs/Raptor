#pragma once

#include <assert.h>

#include <Core/Common.hpp>
#include <Graphics/GLSLProgram.hpp>
#include <Graphics/Color.hpp>
#include <Utils/IO.hpp>
#include <Math/Vector2.hpp>

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

private:
      uint8* memory;
      DebugVertex* vertices;
      uint32* indices;

      GLuint vertexArrayID;
      GLuint vertexBufferID;
      GLuint elementBufferID;
      GLSLProgram* shader;

      bool beginCalled = false;
      uint32 vertexIndex;

      const std::string vertexShaderSource =
      "#version 100\n"
      "attribute vec2 position;"
      "attribute vec4 color;"

      "varying vec4 vertexColor;"

      "uniform mat4 mvp;"
      "void main() {"
        "vertexColor = color;"
        "gl_Position	= mvp * vec4(position.xy, 0.0, 1.0);"
      "}"
      ;


      const std::string fragmentShaderSource =
      "#version 100\n"
      "precision mediump float;"
      "varying lowp vec4 vertexColor;"
      "void main() {"
        "gl_FragColor = vertexColor;"
      "}";
};
