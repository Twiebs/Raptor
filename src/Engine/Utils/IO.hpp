#pragma once

#include <string>
#include <iostream>
#include <fstream>

#include <Core/Common.hpp>
#include <GL/glew.h>

#include <Graphics/GLSLProgram.hpp>
#include <Graphics/Pixmap.hpp>

Pixmap* LoadPixmap(std::string filename);
bool ParseGLSLShader(const std::string& filename, std::string& outFile);

GLuint DEBUGCompileShader(std::string& shaderSource, GLenum shaderType);
GLuint DEBUGLoadShaderFromSource(std::string vertexShaderSource, std::string fragmentShaderSource);
GLuint DEBUGLoadShaderFromFile(const std::string& vertexFilename, const std::string& fragmentFilename);
GLuint DEBUGLoadTexture(std::string filename);
