#pragma once

#include <GL/glew.h>
#include <Core/Common.hpp>

#define GLSL_LOG_SIZE 512

GLint GetUniformLocation(GLuint shaderProgramID, const GLchar* name);
bool ParseGLSLShader(const std::string& filename, std::string& outFile);
GLuint DEBUGCompileShader(std::string& shaderSource, GLenum shaderType);
GLuint DEBUGLoadShaderFromSource(std::string vertexShaderSource, std::string fragmentShaderSource);
GLuint DEBUGLoadShaderFromFile(const std::string& vertexFilename, const std::string& fragmentFilename);