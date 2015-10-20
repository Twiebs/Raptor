#pragma once

#include <GL/glew.h>
#include <Core/Common.hpp>

#define GLSL_LOG_SIZE 512

enum ShaderType {
	VERTEX_SHADER,
	FRAGMENT_SHADER,
	GEOMETRY_SHADER,
	COMPUTE_SHADER,
	SHADER_TYPE_COUNT = 4
};

class GLSLCompiler {
public:

	void addSourceFile (ShaderType type, const std::string& filename);
	void addDefnition (const std::string& str);

	GLuint compile();

	GLSLCompiler();
	~GLSLCompiler();

private:
	std::string shaderSources[SHADER_TYPE_COUNT];
	std::vector<std::string> defnitions;

	std::vector<const char*> sourcesToCompile;
	GLuint compile_shader_type(GLenum shadertype);
};

GLuint CreateShader (const std::string& vertexFilename, const std::string& fragmentFilename);
GLuint CreateShader (const std::string& vertexFilename, const std::string& fragmentFilename, const std::string& geometryFilename);
GLuint CreateShaderFromSource(const char* vertexShaderSource, const char* fragmentShaderSource);
GLuint CreateShaderFromSource(const char* vertexShaderSource, const char* fragmentShaderSource, const char* geometryShaderSource);

GLint GetUniformLocation(GLuint shaderProgramID, const GLchar* name);



