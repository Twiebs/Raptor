#pragma once

#include <vector>
#include <string>
#include <GL\glew.h>
#include <glm\glm.hpp>
#include <Math\Vector3.hpp>

#define SHADER_INFO_LOG_SIZE 512

class GLSLProgram {
public:
	static enum ProgramAttrib {
		POSITION,
		NORMAL,
		UV,
	};
	
	GLSLProgram(GLuint programID);
	~GLSLProgram();

	void Use();
	void Use(std::string target);
	GLuint GetProgram();
	GLint GetUniformLocation(const char* uniformName);

	//Uniform setters
	void SetFloat(const GLchar* name, GLfloat value);
	void SetInt(const GLchar* name, GLint value);
	void SetVector3(const GLchar* name, Vector3& value);
	void SetMatrix4(const GLchar* name, glm::mat4& value);
private:
	GLuint programID;
	std::string tag;
	std::string target;

	//Debug stuff
	std::vector<std::string> unkownUniformLog;
	bool outputOnce = true;
};
