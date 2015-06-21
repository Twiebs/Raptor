#include<iostream>
#include<fstream>
#include<string>

#include "GLSLProgram.h"

GLSLProgram::GLSLProgram(GLuint programID) :
	programID(programID)
{

}

GLSLProgram::~GLSLProgram() {
	std::cout << "Destroying shaderID: " << programID << " | Tag: " << tag << std::endl;
	glDeleteProgram(programID);
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
			"while attempting to render:" << target << std::endl;
		unkownUniformLog.push_back(uniformName);
	}
	return location;
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
