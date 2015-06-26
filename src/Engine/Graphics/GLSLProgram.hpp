#pragma once

#include <vector>
#include <string>
#include <GL/glew.h>
#include <Math/Vector3.hpp>
#include <Math/Matrix4.hpp>

#define SHADER_INFO_LOG_SIZE 512

class GLSLProgram {
public:

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
	void SetMatrix4(const GLchar* name, Matrix4& matrix);

private:
	GLuint programID;
	std::string tag;
	std::string target;

	//Debug stuff
	std::vector<std::string> unkownUniformLog;
	bool outputOnce = true;
};
