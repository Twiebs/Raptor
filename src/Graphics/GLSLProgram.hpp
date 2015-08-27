#pragma once

#include <GL/glew.h>
#include <Core/Common.hpp>

#define GLSL_LOG_SIZE 512

//TODO Merge Shader Parsing and compilation into a singluar function
//TODO consider moving all of this functionality into a Renderer.hpp
//That file wont contain anything called a Renderer though!

struct GLSLProgram {
	GLuint shaderProgramID = 0;
	
	GLSLProgram();
	GLSLProgram(const std::string& vertexFilename, const std::string& fragmentFilename);
	GLSLProgram(const std::string& vertexFilename, const std::string& fragmentFilename, const std::string& geometryFilename);
	~GLSLProgram();

	GLint GetUniformLocation(const std::string& name);

	void Load(const std::string& vertexFilename, const std::string& fragmentFilename);
	void Load(const std::string& vertexFilename, const std::string& fragmentFilename, const std::string& geometryFilename);

	void Use();
	void Dispose();
};

GLint GetUniformLocation(GLuint shaderProgramID, const GLchar* name);


bool ParseGLSLShader(const std::string& filename, std::string& outFile);
GLuint CompileShader(const char* shaderSource, GLenum shaderType);
GLuint LoadShaderFromSource(const char* vertexShaderSource, const char* fragmentShaderSource);
GLuint LoadShaderFromSource(const char* vertexShaderSource, const char* fragmentShaderSource, const char* geometryShaderSource);
GLuint LoadShaderFromFile(const std::string& vertexFilename, const std::string& fragmentFilename);
GLuint LoadShaderFromFile(const std::string& vertexFilename, const std::string& fragmentFilename, const std::string& geometryFilename);
