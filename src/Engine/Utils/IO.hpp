#pragma once

#include <string>
#include <iostream>
#include <fstream>

#include <Core/Common.hpp>
#include <GL/glew.h>
#include <Graphics/GLSLProgram.hpp>

//DEBUGstuff
inline uint32 DEBUGCompileShader(const std::string& shaderSource, GLenum shaderType) {
  const GLchar* source = shaderSource.c_str();
  GLuint shaderID = glCreateShader(shaderType);
  glShaderSource(shaderID, 1, &source, NULL);
  glCompileShader(shaderID);

  GLint success;
  GLchar infoLog[SHADER_INFO_LOG_SIZE];
  glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);

  if (!success) {
    glGetShaderInfoLog(shaderID, SHADER_INFO_LOG_SIZE, NULL, infoLog);
    std::string shadeTypeName = (shaderType == GL_VERTEX_SHADER) ? ("VertexShader") : ("FragmentShader");
    LOG_ERROR(shadeTypeName << " compilation failed! \n" << infoLog);
    return 0;
  }
  return shaderID;
}

inline GLSLProgram* DEBUGLoadShaderFromSource(const std::string& vertexShaderSource, const std::string& fragmentShaderSource) {
  GLuint vertexShaderID = DEBUGCompileShader(vertexShaderSource, GL_VERTEX_SHADER);
  GLuint fragmentShaderID = DEBUGCompileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

  GLuint programID = glCreateProgram();
  glAttachShader(programID, vertexShaderID);
  glAttachShader(programID, fragmentShaderID);
  glLinkProgram(programID);

  GLint success;
  GLchar infoLog[SHADER_INFO_LOG_SIZE];
  glGetProgramiv(programID, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(programID, SHADER_INFO_LOG_SIZE, NULL, infoLog);
    LOG_ERROR("SHADER PROGRAM LINK: " << infoLog);
    return nullptr;
  }
  glDetachShader(programID, vertexShaderID);
  glDetachShader(programID, fragmentShaderID);
  glDeleteShader(vertexShaderID);
  glDeleteShader(fragmentShaderID);

  return new GLSLProgram(programID);
}
