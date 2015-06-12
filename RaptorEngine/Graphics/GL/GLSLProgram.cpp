#include<iostream>
#include<fstream>
#include<string>

#include "GLSLProgram.h"
#include <Core\EngineUtils.h>


GLSLProgram::GLSLProgram() {

}

GLSLProgram::GLSLProgram(const char* vertexSourcePath, const char* fragmentSourcePath) {
	Init(vertexSourcePath, fragmentSourcePath);
}

GLSLProgram::~GLSLProgram() {
	cout << "Destroying shaderID: " << programID << " | Tag: " << tag << endl;
	glDeleteProgram(programID);
}

void GLSLProgram::Init(const char* vertexSourcePath, const char* fragmentSourcePath) {
	this->tag = vertexSourcePath;
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

	glDetachShader(programID, vertexShaderID);
	glDetachShader(programID, fragmentShaderID);
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
}

GLint GLSLProgram::GetUniformLocation(const char* uniformName) {
	GLint location = glGetUniformLocation(programID, uniformName);
	if (location == GL_INVALID_INDEX) {
		for (std::string log : unkownUniformLog) {
			if (log.compare(uniformName) == 0) {
				return location;
			}
		}
		std::cout << "ERROR: Uniform[" << uniformName << "] unreconized(optimized) in GLSLProgram[" << programID << "]" <<
			"while attempting to render:" << target << endl;
		unkownUniformLog.push_back(uniformName);
	}
	return location;
}




GLuint GLSLProgram::CompileShader(string shaderSource, GLenum shaderType) {
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

void GLSLProgram::Use() {
	this->target = "UNSPECIFIED";
	glUseProgram(programID);
}

void GLSLProgram::Use(std::string target) {
	this->target = target;
	glUseProgram(programID);
}

GLuint GLSLProgram::GetProgram() {
	return programID;
}


void GLSLProgram::SetFloat(const GLchar* name, GLfloat value){
	glUniform1f(GetUniformLocation(name), value);
}
void GLSLProgram::SetInt(const GLchar* name, GLint value){
	glUniform1i(GetUniformLocation(name), value);
}
void GLSLProgram::SetVector3(const GLchar* name, Vector3& value){
	glUniform3f(GetUniformLocation(name), value.x, value.y, value.z);
}
void GLSLProgram::SetMatrix4(const GLchar* name, glm::mat4& value){
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &value[0][0]);
}
