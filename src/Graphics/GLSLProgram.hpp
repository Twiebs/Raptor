#pragma once

#include <vector>

#include <Core/config.h>
#include <Core/types.h>


#include <GL/glew.h>

#ifdef GENERATE_SHADER_SOURCE_FILES
#define BeginShaderBuilder(name) ShaderBuilder _builder (name)
#define AddShaderSourceFile(type, filename) _builder.addSourceFile(type, filename)
#define AddShaderConstant(string) _builder.addString(string)
#define AddShaderString(string) _builder.addString(string)
#define ReturnBuiltShaderID() _builder.build()
#else
#define BeginShaderBuilder(name) ShaderBuilder _builder (name)
#define AddShaderSourceFile(type, filename)
#define AddShaderConstant(string)
#define AddShaderString(string)
#define ReturnBuiltShaderHandle() _builder.build()
#endif

enum ShaderType {
	VERTEX_SHADER,
	FRAGMENT_SHADER,
	GEOMETRY_SHADER,
	COMPUTE_SHADER,
	SHADER_TYPE_COUNT = 4
};

struct Shader {
	GLuint id;
};

struct ShaderBuilderData {
	std::string name;
	std::string sourceFilenames[SHADER_TYPE_COUNT];
	std::vector <std::string> addedStrings;
};

class ShaderBuilder {
public:
	ShaderBuilder(const std::string& shaderName);
	~ShaderBuilder();

	void addSourceFile(ShaderType type, const std::string& filename);
	void addString(const std::string& str);
	void addConstant(const std::string& str);

	GLuint build();
	Shader build_program();

#ifdef GENERATE_SHADER_SOURCE_FILES
	ShaderBuilderData data;
#else

private:

	std::string name;
#endif
};

GLuint CompileShader(ShaderBuilderData* info);

GLuint CreateShader (const std::string& vertexFilename, const std::string& fragmentFilename);
GLuint CreateShader (const std::string& vertexFilename, const std::string& fragmentFilename, const std::string& geometryFilename);
GLuint CreateShaderFromSource(const char* vertexShaderSource, const char* fragmentShaderSource);
GLuint CreateShaderFromSource(const char* vertexShaderSource, const char* fragmentShaderSource, const char* geometryShaderSource);
GLint GetUniformLocation(GLuint shaderProgramID, const GLchar* name);



