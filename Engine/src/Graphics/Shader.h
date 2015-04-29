#pragma once

#include<string>

#include<GL\glew.h>

#define SHADER_INFO_LOG_SIZE 512

class ShaderProgram {
public:
	static enum ProgramAttrib {
		POSITION,
		NORMAL,
		TEXCOORD,
	};
	
	ShaderProgram();
	ShaderProgram(const char* vertexShaderPath, const char* fragmentShaderPath);
	~ShaderProgram();

	void Init(const char* vertexShaderPath, const char* fragmentShaderPath);

	void Use();
	GLuint GetProgram();
	GLint GetUniformLocation(const char* uniformName);

private:
	GLuint programID;

	//Returns compiled shaderID
	GLuint CompileShader(const std::string shaderSource, GLenum shaderType);
};

