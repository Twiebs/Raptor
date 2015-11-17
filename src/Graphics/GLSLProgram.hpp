#pragma once

#include <vector>

#include <Core/config.h>
#include <Core/types.h>


#include <GL/glew.h>

#ifdef COMPILE_SHADERS_AT_RUNTIME
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

enum ShaderFlags {
	SHADERFLAG_VERTEXSHADER		= 1 << 0,
	SHADERFLAG_FRAGMENTSHADER	= 1 << 1,
	SHADERFLAG_GEOMETRYSHASDER	= 1 << 2,
	SHADERFLAG_COMPUTESHADER	= 1 << 3,
};

U64 GetShaderTypeFlag(ShaderType type);

struct Shader {
	GLuint id;
	std::string shaderName;
};

struct ShaderBuilderData {
	std::string name;
	std::string sourceFilenames[SHADER_TYPE_COUNT];
	std::vector <std::string> addedStrings;
	U64 flags = 0;
	bool hotreloadEnabled = false;
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

	ShaderBuilderData data;
};

GLuint CompileShader(ShaderBuilderData* info);

Shader CreateShaderObject (const std::string& vertexFilename, const std::string& fragmentFilename);
GLuint CreateShader (const std::string& vertexFilename, const std::string& fragmentFilename);
GLuint CreateShader (const std::string& vertexFilename, const std::string& fragmentFilename, const std::string& geometryFilename);
GLuint CreateShaderFromSource(const char* vertexShaderSource, const char* fragmentShaderSource);
GLuint CreateShaderFromSource(const char* vertexShaderSource, const char* fragmentShaderSource, const char* geometryShaderSource);
GLint GetUniformLocation(GLuint shaderProgramID, const GLchar* name);



