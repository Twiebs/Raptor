#pragma once

#include <GL/glew.h>
#include <Math/Vector3.hpp>
#include <Math/Matrix4.hpp>

#define GLSL_LOG_SIZE 512

GLint GetUniformLocation(GLuint shaderProgramID, const GLchar* name);