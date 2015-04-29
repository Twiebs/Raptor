#include<string>

#include "Technique.h"
#include<Core\EngineUtils.h>

Technique::Technique() {
	shaderProgram = 0;
}


Technique::~Technique() {
	for (ShaderIDList::iterator iter = shaderIDList.begin(); iter != shaderIDList.end(); iter++) {
		glDeleteShader(*iter);
	}

	if (shaderProgram != 0) {
		glDeleteProgram(shaderProgram);
		shaderProgram = 0;
	}
}

void Technique::Enable() {
	glUseProgram(shaderProgram);
}

bool Technique::Init() {
	shaderProgram = glCreateProgram();
	if (shaderProgram == 0) {
		return false;
	}
	return true;
}

bool Technique::AddShader(GLenum shaderType, const char* filename) {
	string parsedFile;

	if (!ReadFile(filename, parsedFile))
		return false;

	GLuint shaderID = glCreateShader(shaderType);
	if (shaderID = 0) {
		LOG_DEFAULT("Error creating shaderType << shaderType");	//XXX error msg
		return false;
	}

	shaderIDList.push_back(shaderID);

	const GLchar* p[1];
	p[0] = parsedFile.c_str();
	
	GLint lengths[1] = {
		(GLint)parsedFile.size()
	};

	glShaderSource(shaderID, 1, p, lengths);
	glCompileShader(shaderID);

	GLint sucuess;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &sucuess);

	if (!sucuess) {
		GLchar infoLog[1024];
		glGetShaderInfoLog(shaderID, 1024, NULL, infoLog);
		LOG_DEFAULT("Error Compiling!");
		return false;
	}

	glAttachShader(shaderProgram, shaderID);
	return true;
}

bool Technique::Finalize() {
	GLint sucuess = 0;
	GLchar errorLog[1024] = { 0 };

	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &sucuess);

	if (sucuess == 0) {
		glGetProgramInfoLog(shaderProgram, sizeof(errorLog), NULL, errorLog);
		return false;
	}

	glValidateProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_VALIDATE_STATUS, &sucuess);

	if (sucuess == 0) {
		glGetProgramInfoLog(shaderProgram, sizeof(errorLog), NULL, errorLog);
		return false;
	}

	for (ShaderIDList::iterator iter = shaderIDList.begin(); iter != shaderIDList.end(); iter++) {
		glDeleteShader(*iter);
	}

	shaderIDList.clear();

	return true;
}

GLint Technique::GetUniformLocation(const char* uniformName) {
	GLint location = glGetUniformLocation(shaderProgram, uniformName);

	//TODO get uniform location error message
	return location;
}

