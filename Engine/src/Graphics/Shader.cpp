#include<iostream>
#include<fstream>
#include<string>

#include "Shader.h"
#include <Core\EngineUtils.h>
#include<Core\Engine.h>

using namespace std;

ShaderProgram::ShaderProgram() {

}

ShaderProgram::ShaderProgram(const char* vertexSourcePath, const char* fragmentSourcePath) {
	Init(vertexSourcePath, fragmentSourcePath);
}

ShaderProgram::~ShaderProgram() {
	cout << "Destroying shaderID: " << programID;
	glDeleteProgram(programID);
}

void ShaderProgram::Init(const char* vertexSourcePath, const char* fragmentSourcePath) {
	string vertexShaderSource;
	string fragmentShaderSource;

	ReadFile(vertexSourcePath, vertexShaderSource);
	ReadFile(fragmentSourcePath, fragmentShaderSource);

	GLuint vertexShaderID = CompileShader(vertexShaderSource, GL_VERTEX_SHADER);
	GLuint fragmentShaderID = CompileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

	programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	GLint success;
	GLchar infoLog[SHADER_INFO_LOG_SIZE];
	glGetProgramiv(programID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(programID, SHADER_INFO_LOG_SIZE, NULL, infoLog);
		std::cout << "ERROR: SHADER PROGRAM LINK:: " << infoLog << std::endl;
	}

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
}

GLint ShaderProgram::GetUniformLocation(const char* uniformName) {
	return glGetUniformLocation(programID, uniformName);
}


GLuint ShaderProgram::CompileShader(string shaderSource, GLenum shaderType) {
	//Convert source string to GLchar*
	const GLchar* source = shaderSource.c_str();

	//Create and compile shader
	GLuint shaderID = glCreateShader(shaderType);
	glShaderSource(shaderID, 1, &source, NULL);
	glCompileShader(shaderID);

	//Check sucuess
	GLint success;
	GLchar infoLog[SHADER_INFO_LOG_SIZE];
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(shaderID, SHADER_INFO_LOG_SIZE, NULL, infoLog);
		string shadeTypeName = (shaderType == GL_VERTEX_SHADER) ? ("VertexShader") : ("FragmentShader");
		cout << "ERROR: " << shadeTypeName << " compilation failed! \n" << infoLog << endl;
		return 0;

	}

	return shaderID;
}

void ShaderProgram::Use() {
	glUseProgram(programID);
}

GLuint ShaderProgram::GetProgram() {
	return programID;
}