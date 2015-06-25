#pragma once

#include <string>
#include <iostream>
#include <fstream>

#include <Core/Common.hpp>
#include <GL/glew.h>

#include <Graphics/GLSLProgram.hpp>
#include <Graphics/Pixmap.hpp>
#include <Platform/Platform.hpp>

bool ParseGLSLShader(const std::string& filename, std::string& outFile);
//DEBUGstuff
GLuint DEBUGCompileShader(std::string& shaderSource, GLenum shaderType);
GLSLProgram* DEBUGLoadShaderFromSource(std::string& vertexShaderSource, std::string& fragmentShaderSource);
GLSLProgram* DEBUGLoadShaderFromFile(const std::string& vertexFilename, const std::string& fragmentFilename);

GLuint DEBUGLoadTexture(std::string filename);
