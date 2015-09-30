#pragma once

#include <GL/glew.h>
#include <Core/Common.hpp>

#define GLSL_LOG_SIZE 512

GLuint CreateShader (const std::string& vertexFilename, const std::string& fragmentFilename);
GLuint CreateShader (const std::string& vertexFilename, const std::string& fragmentFilename, const std::string& geometryFilename);
GLuint CreateShaderFromSource(const char* vertexShaderSource, const char* fragmentShaderSource);
GLuint CreateShaderFromSource(const char* vertexShaderSource, const char* fragmentShaderSource, const char* geometryShaderSource);

GLint GetUniformLocation(GLuint shaderProgramID, const GLchar* name);



