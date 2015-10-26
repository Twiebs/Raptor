#include "GLSLProgram.hpp"
#include <Utils/utils.hpp>

#include <fstream>

#include <Core/logging.h>

// TODO cleanup Shader creation error handeling to use
// the same CheckShaderErrors function!

#define GLSL_ERROR_LOG_SIZE 512
#define GLSL_VERSION_STRING "#version 450 core\n"

static inline GLenum GetGLShaderType (ShaderType type) {
	if (type == VERTEX_SHADER)	 return GL_VERTEX_SHADER;
	if (type == FRAGMENT_SHADER) return GL_FRAGMENT_SHADER;
	if (type == GEOMETRY_SHADER) return GL_GEOMETRY_SHADER;
	if (type == COMPUTE_SHADER)  return GL_COMPUTE_SHADER;
	assert (false && "Unknown Shader Type");
	return 0;
}

static inline const char* GetShaderTypeString (GLenum shaderType) {
	if (shaderType == GL_VERTEX_SHADER) 	return "Vertex Shader";
	if (shaderType == GL_FRAGMENT_SHADER) 	return "Fragment Shader";
	if (shaderType == GL_GEOMETRY_SHADER) 	return "Geometry Shader";
	if (shaderType == GL_COMPUTE_SHADER) 	return "Compute Shader";
	assert (false && "Unknown Shader Type");
	return "UNKNOWN SHADER TYPE";
}

static inline const std::string GetGLShaderExtentionString (ShaderType type) {
	if (type == VERTEX_SHADER)		return ".vert";
	if (type == FRAGMENT_SHADER)	return ".frag";
	if (type == GEOMETRY_SHADER)	return ".geom";
	if (type == COMPUTE_SHADER)		return ".comp";
	assert(false && "Unknown Shader Type");
	return "INVALID_SHADER_EXENTION";
}

static GLuint CheckShaderErrors (GLuint shaderID, GLenum errorType, GLenum shaderType) {
	GLint success;
	GLchar infoLog[GLSL_ERROR_LOG_SIZE];
	glGetShaderiv(shaderID, errorType, &success);
	if (success) return shaderID;

	glGetShaderInfoLog(shaderID, GLSL_ERROR_LOG_SIZE, NULL, infoLog);
	if (errorType == GL_COMPILE_STATUS) {
		LOG_ERROR(GetShaderTypeString(shaderType) << " compilation failed! \n" << infoLog);
	} else {
		LOG_ERROR("SHADER PROGRAM LINK FAILED: " << infoLog);
	}
	return 0;
}

static GLuint LinkShaders (GLuint vertexShaderID, GLuint fragmentShaderID) {
	GLuint programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	GLint success;
	GLchar infoLog[GLSL_ERROR_LOG_SIZE];
	glGetProgramiv(programID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(programID, GLSL_ERROR_LOG_SIZE, NULL, infoLog);
		LOG_ERROR("SHADER PROGRAM LINK: " << infoLog);
		return 0;
	}

	glDetachShader(programID, vertexShaderID);
	glDetachShader(programID, fragmentShaderID);
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
	return programID;
}


GLuint CreateShader (const std::string& vertexFilename, const std::string& fragmentFilename) {
	auto vertexSource = ReadEntireFile(vertexFilename);
	auto fragmentSource = ReadEntireFile(fragmentFilename);
	return CreateShaderFromSource(vertexSource.c_str(), fragmentSource.c_str());
}

GLuint CreateShader (const std::string& vertexFilename, const std::string& fragmentFilename, const std::string& geometryFilename) {
	auto vertexSource = ReadEntireFile(vertexFilename);
	auto fragmentSource = ReadEntireFile(fragmentFilename);
	auto geometrySource = ReadEntireFile(geometryFilename);
	return CreateShaderFromSource(vertexSource.c_str(), fragmentSource.c_str(), geometrySource.c_str());
}

GLint GetUniformLocation (GLuint shaderProgramID, const GLchar* name) {
	GLint result = glGetUniformLocation(shaderProgramID, name);
#if 0
	if (result == -1) { LOG_ERROR(name << " is not a valid uniform variable!"); }
	else if (result == GL_INVALID_INDEX) { LOG_ERROR(name << " is not a uniform variable(may have been optimized out by GLSL"); }
	else if (result == GL_INVALID_VALUE) { LOG_ERROR("GPU_Program was not a value generated by OpenGL when trying to get location of uniform" << name); }
	else if (result == GL_INVALID_OPERATION) { LOG_ERROR("Invalid shader program provided when getting location of uniform " << name); }
#endif
	return result;
}

static GLuint CompileShader (const char* source, GLenum shaderType) {
	GLuint shaderID = glCreateShader(shaderType);
	glShaderSource(shaderID, 1, &source, NULL);
	glCompileShader(shaderID);

	GLint success;
	GLchar infoLog[GLSL_ERROR_LOG_SIZE];
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shaderID, GLSL_ERROR_LOG_SIZE, NULL, infoLog);
		LOG_ERROR(GetShaderTypeString(shaderType) << " compilation failed! \n" << infoLog);
		printf("Shader Source: %s", source);
		return 0;
	}
	return shaderID;
}

GLuint CreateShaderFromSource (const char* vertexShaderSource, const char* fragmentShaderSource) {
	GLuint vertexShaderID = CompileShader(vertexShaderSource, GL_VERTEX_SHADER);
	GLuint fragmentShaderID = CompileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

	GLuint programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	GLint success;
	GLchar infoLog[GLSL_ERROR_LOG_SIZE];
	glGetProgramiv(programID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(programID, GLSL_ERROR_LOG_SIZE, NULL, infoLog);
		LOG_ERROR("SHADER PROGRAM LINK: " << infoLog);
		return 0;
	}

	glDetachShader(programID, vertexShaderID);
	glDetachShader(programID, fragmentShaderID);
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
	return programID;
}

GLuint CreateShaderFromSource (const char* vertexShaderSource, const char* fragmentShaderSource, const char* geomertyShaderSource) {
	GLuint vertexShaderID = CompileShader(vertexShaderSource, GL_VERTEX_SHADER);
	GLuint fragmentShaderID = CompileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);
	GLuint geomertyShaderID = CompileShader(geomertyShaderSource, GL_GEOMETRY_SHADER);

	GLuint programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glAttachShader(programID, geomertyShaderID);
	glLinkProgram(programID);

	GLint success;
	GLchar infoLog[GLSL_ERROR_LOG_SIZE];
	glGetProgramiv(programID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(programID, GLSL_ERROR_LOG_SIZE, NULL, infoLog);
		LOG_ERROR("SHADER PROGRAM LINK: " << infoLog);
		return 0;
	}

	glDetachShader(programID, vertexShaderID);
	glDetachShader(programID, fragmentShaderID);
	glDetachShader(programID, geomertyShaderID);
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
	glDeleteShader(geomertyShaderID);
	return programID;
}

static GLuint CompileShader(ShaderType type, const std::vector<const char*>& sources) {
	GLenum glShaderType = GetGLShaderType(type);
	GLuint shaderID = glCreateShader(glShaderType);
	glShaderSource(shaderID, sources.size(), &sources.front(), NULL);
	glCompileShader(shaderID);
	return CheckShaderErrors(shaderID, GL_COMPILE_STATUS, glShaderType);
}

struct ShaderParser {
	char* currentChar;
};

//static bool ExpectStringAndEatIfTrue (ShaderParser* parser, const char* string) {
//	char* currentStringChar = (char*)string;
//	size_t stringLength = strlen(string);
//	size_t currentStringIndex = 0;
//	while ()
//
//}

static std::string ParseGLSLShaderFile (const std::string& filename) {
	ShaderParser parser;

	auto source = ReadEntireFile(filename);
	parser.currentChar = &source.front();



	bool isParsing = false;
	while (isParsing) {
	switch (*parser.currentChar) {
	case '#':
	{

	} break;


	case EOF:
	{
		isParsing = false;
		break;
	} break;

	}
	parser.currentChar++;
	}
}

static void GenerateAndWriteShaderSourceFile(ShaderType type, const std::string& name, const std::string& source, const std::vector<std::string>& stringsToInclude) {
	auto generated_filename = GENERATE_SHADER_SOURCE_FILE_DIRECTORY + name + GetGLShaderExtentionString(type);
	FILE* file = fopen(generated_filename.c_str(), "w");
	if (file == nullptr) {
		perror(generated_filename.c_str());
		LOG_ERROR("Could not open file [" << generated_filename << "] when emitting auto generated shader source");
		return;
	}

	for (auto& str : stringsToInclude) {
		fputs(str.c_str(), file);
	}

	fputs(source.c_str(), file);
	fclose(file);

	LOG_INFO("Generated " << generated_filename << " shader source file");

	//
	//if ( !stream.is_open() ) {
	//	LOG_ERROR("Could not open file when generating shader source!");
	//	return;
	//}

	//for (auto& str : stringsToInclude) {
	//	stream.write(str.c_str(), str.length());
	//}

	//stream.write(source.c_str(), source.length());
	//stream.close();
}




static inline GLuint CheckShaderErrors(GLuint shaderID, GLenum errorType, GLenum shaderType = 0);

ShaderBuilder::~ShaderBuilder() { }

#ifndef GENERATE_SHADER_SOURCE_FILES
ShaderBuilder::ShaderBuilder(const std::string& name) : name(name) { }
void ShaderBuilder::addString(const std::string& string) { }
void ShaderBuilder::addSourceFile(ShaderType type, const std::string& filename) { }
void ShaderBuilder::addConstant(const std::string& str) { }

GLSLProgram ShaderBuilder::build() {
	GLuint shaderID = CreateShader(GENERATE_SHADER_SOURCE_FILE_DIRECTORY + name + GetGLShaderExtention(VERTEX_SHADER),
		GENERATE_SHADER_SOURCE_FILE_DIRECTORY + name + GetGLShaderExtention(FRAGMENT_SHADER));
	return GLSLProgram{ shaderID };
}
#else

ShaderBuilder::ShaderBuilder (const std::string& name) {
	data.name = name;
	addString(GLSL_VERSION_STRING);
}

void ShaderBuilder::addSourceFile(ShaderType type, const std::string& filename) {
	data.sourceFilenames[type] = filename;
}

void ShaderBuilder::addString(const std::string& str) {
	data.addedStrings.push_back(str);
}

void ShaderBuilder::addConstant(const std::string& str) {

}

GLuint ShaderBuilder::build() {
	return CompileShader(&data);
}

Shader ShaderBuilder::build_program() {
	return Shader{ CompileShader(&data) };
}

GLuint CompileShader(ShaderBuilderData* info) {
	std::vector<const char*> sourcesToCompile;
	for (auto& str : info->addedStrings)
		sourcesToCompile.push_back(str.c_str());

	GLuint shaders[SHADER_TYPE_COUNT];
	std::string sources[SHADER_TYPE_COUNT];
	for (U32 i = 0; i < SHADER_TYPE_COUNT; i++) {
		if (info->sourceFilenames[i].size() > 0) {
			sources[i] = ReadEntireFile(info->sourceFilenames[i]);
			sourcesToCompile.push_back(sources[i].c_str());
			shaders[i] = CompileShader((ShaderType)i, sourcesToCompile);
			sourcesToCompile.pop_back();
		}
		else {
			shaders[i] = 0;
		}
	}

	GLuint programID = glCreateProgram();
	for (U32 i = 0; i < SHADER_TYPE_COUNT; i++) {
		if (shaders[i] != 0) glAttachShader(programID, shaders[i]);
	}
	glLinkProgram(programID);

	for (U32 i = 0; i < SHADER_TYPE_COUNT; i++) {
		if (shaders[i] != 0) {
			glDetachShader(programID, shaders[i]);
			glDeleteShader(shaders[i]);
		}
	}

	for (U32 i = 0; i < SHADER_TYPE_COUNT; i++) {
		if (sources[i].size() == 0) break;
		GenerateAndWriteShaderSourceFile((ShaderType)i, info->name, sources[i], info->addedStrings);
	}

	auto result = CheckShaderErrors(programID, GL_LINK_STATUS, 0);
	if (result != 0) LOG_VERBOSE("ShaderProgram " << info->name << " was linked sucuessfuly!");
	return result;
}

#endif

