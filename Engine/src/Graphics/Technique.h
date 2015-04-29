#pragma once

#include<list>
#include<GL\glew.h>

typedef std::list<GLuint> ShaderIDList;

class Technique {
public:
	Technique();
	~Technique();
	virtual bool Init();
	void Enable();
protected:
	bool AddShader(GLenum shaderType, const char* shaderText);
	bool Finalize();
	GLint GetUniformLocation(const char* uniformName);
private:
	GLuint shaderProgram;
	ShaderIDList shaderIDList;
};

