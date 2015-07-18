#pragma once

#include <GL/glew.h>
#include <Core/Common.hpp>

#define GLSL_LOG_SIZE 512

GLint GetUniformLocation(GLuint shaderProgramID, const GLchar* name);

bool ParseGLSLShader(const std::string& filename, std::string& outFile);
GLuint CompileShader(const char* shaderSource, GLenum shaderType);
GLuint LoadShaderFromSource(const char* vertexShaderSource, const char* fragmentShaderSource);
GLuint DEBUGLoadShaderFromFile(const std::string& vertexFilename, const std::string& fragmentFilename);
