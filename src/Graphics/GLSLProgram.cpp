#include "GLSLProgram.hpp"
#include <Utils/utils.hpp>

static inline GLuint CheckShaderErrors (GLuint shaderID, GLenum errorType, GLenum shaderType = 0);

GLSLCompiler::GLSLCompiler() {
	addDefnition("#version 450 core\n");
}

GLSLCompiler::~GLSLCompiler() { }

void GLSLCompiler::addSourceFile(ShaderType type, const std::string& filename) {
	shaderSources[type] = ReadEntireFile(filename);
}

void GLSLCompiler::addDefnition (const std::string& str) {
	defnitions.push_back(str);
	sourcesToCompile.push_back(defnitions.back().c_str());
}

static inline GLenum GetGLShaderType (ShaderType type) {
	if (type == VERTEX_SHADER) return GL_VERTEX_SHADER;
	if (type == FRAGMENT_SHADER) return GL_FRAGMENT_SHADER;
	if (type == GEOMETRY_SHADER) return GL_GEOMETRY_SHADER;
	if (type == COMPUTE_SHADER) return GL_COMPUTE_SHADER;
}

GLuint GLSLCompiler::compile() {
	GLuint shaders[SHADER_TYPE_COUNT];

	for (U32 i = 0; i < SHADER_TYPE_COUNT; i++) {
		if (shaderSources[i].size() > 0) {
			sourcesToCompile.push_back(shaderSources[i].c_str());
			shaders[i] = compile_shader_type(GetGLShaderType((ShaderType)i));
			sourcesToCompile.pop_back();
		} else {
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

	return CheckShaderErrors(programID, GL_LINK_STATUS, 0);
}



GLuint GLSLCompiler::compile_shader_type (GLenum shaderType) {
	GLuint shaderID = glCreateShader(shaderType);
	glShaderSource(shaderID, sourcesToCompile.size(), &sourcesToCompile.front(), NULL);
	glCompileShader(shaderID);
	return CheckShaderErrors(shaderID, GL_COMPILE_STATUS, shaderType);
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
	if (result == -1) { LOG_ERROR(name << " is not a valid uniform variable!"); }
	else if (result == GL_INVALID_INDEX) { LOG_ERROR(name << " is not a uniform variable(may have been optimized out by GLSL"); }
	else if (result == GL_INVALID_VALUE) { LOG_ERROR("GPU_Program was not a value generated by OpenGL when trying to get location of uniform" << name); }
	else if (result == GL_INVALID_OPERATION) { LOG_ERROR("Invalid shader program provided when getting location of uniform " << name); }
	return result;
}

#if 0
static bool ParseGLSLShader (const std::string& shaderSource) {
    std::string line;
	while (getline(shaderSource, line)) {
		// Check if the file has a include directive
		if (line.find("#") != std::string::npos) {
			if (line.find("include") != std::string::npos) {
				size_t begin = line.find("<") + 1;
				size_t end = line.find(">") - begin;
				std::string includeFilename = line.substr(begin, end);
				auto baseFilepath = filename.substr(0, filename.find_last_of('/'));
				baseFilepath.append("/");
				baseFilepath.append(includeFilename);
				ParseGLSLShader(baseFilepath, includeFile);
				outFile.append(includeFile);
				continue;	//Dont add the include line...
			}
		}
		outFile.append(line);
		outFile.append("\n");
	}

	fileStream.close();
	return true;
}
#endif


static inline const char* ShaderTypeToString (GLenum shaderType) {
	if (shaderType == GL_VERTEX_SHADER) 	return "Vertex Shader";
	if (shaderType == GL_FRAGMENT_SHADER) 	return "Fragment Shader";
	if (shaderType == GL_GEOMETRY_SHADER) 	return "Geometry Shader";
	if (shaderType == GL_COMPUTE_SHADER) 	return "Compute Shader";
	return "Unknown Shader Type";
}

static inline GLuint CheckShaderErrors (GLuint shaderID, GLenum errorType, GLenum shaderType) {
	static const int LOG_SIZE = 512;
	GLint success;
	GLchar infoLog[LOG_SIZE];
	glGetShaderiv(shaderID, errorType, &success);
	if (success == false) {
		glGetShaderInfoLog(shaderID, LOG_SIZE, NULL, infoLog);
		if (errorType == GL_COMPILE_STATUS) {
			LOG_ERROR(ShaderTypeToString(shaderType) << " compilation failed! \n" << infoLog);
		} else {
			LOG_ERROR("SHADER PROGRAM LINK FAILED: " << infoLog);
		}
		return 0;
	}
	return shaderID;
}

static GLuint CompileShader (const char* source, GLenum shaderType) {
	GLuint shaderID = glCreateShader(shaderType);
	glShaderSource(shaderID, 1, &source, NULL);
	glCompileShader(shaderID);

	GLint success;
	GLchar infoLog[GLSL_LOG_SIZE];
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shaderID, GLSL_LOG_SIZE, NULL, infoLog);
		LOG_ERROR(ShaderTypeToString(shaderType) << " compilation failed! \n" << infoLog);
        printf("Shader Source: %s", source);
		return 0;
	}
	return shaderID;
}

GLuint CreateShaderFromSource (const char* vertexShaderSource, const char* fragmentShaderSource) {
	GLuint vertexShaderID   = CompileShader(vertexShaderSource, GL_VERTEX_SHADER);
	GLuint fragmentShaderID = CompileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

	GLuint programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	GLint success;
	GLchar infoLog[GLSL_LOG_SIZE];
	glGetProgramiv(programID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(programID, GLSL_LOG_SIZE, NULL, infoLog);
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
	GLchar infoLog[GLSL_LOG_SIZE];
	glGetProgramiv(programID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(programID, GLSL_LOG_SIZE, NULL, infoLog);
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

static GLuint LinkShaders(GLuint vertexShaderID, GLuint fragmentShaderID) {
	GLuint programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	GLint success;
	GLchar infoLog[GLSL_LOG_SIZE];
	glGetProgramiv(programID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(programID, GLSL_LOG_SIZE, NULL, infoLog);
		LOG_ERROR("SHADER PROGRAM LINK: " << infoLog);
		return 0;
}

	glDetachShader(programID, vertexShaderID);
	glDetachShader(programID, fragmentShaderID);
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
	return programID;
}


