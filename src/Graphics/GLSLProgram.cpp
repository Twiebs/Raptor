#include "GLSLProgram.hpp"

#include <fstream>


GLSLProgram::GLSLProgram() {

}

GLSLProgram::GLSLProgram(const std::string& vertexFilename, const std::string& fragmentFilename) {
	Load(vertexFilename, fragmentFilename);
}

GLSLProgram::GLSLProgram(const std::string& vertexFilename, const std::string& fragmentFilename, const std::string& geometryFilename) {
	Load(vertexFilename, fragmentFilename, geometryFilename);
}


GLSLProgram::~GLSLProgram() {
	//Dispose();	//This is so fucking stupid
}

void GLSLProgram::Use() {
	glUseProgram(shaderProgramID);
}

void GLSLProgram::Dispose() {
	glDeleteProgram(shaderProgramID);
	shaderProgramID = 0;
}



void GLSLProgram::Load(const std::string& vertexFilename, const std::string& fragmentFilename) {
	shaderProgramID = LoadShaderFromFile(vertexFilename, fragmentFilename);
}

void GLSLProgram::Load(const std::string& vertexFilename, const std::string& fragmentFilename, const std::string& geometryFilename) {
	shaderProgramID = LoadShaderFromFile(vertexFilename, fragmentFilename, geometryFilename);
}


GLint GLSLProgram::GetUniformLocation(const std::string& name) {
	GLint result = glGetUniformLocation(shaderProgramID, name.c_str());
	if (result == -1) { LOG_ERROR(name << " is not a valid uniform variable!"); }
	else if (result == GL_INVALID_INDEX) { LOG_ERROR(name << " is not a uniform variable(may have been optimized out by GLSL"); }
	else if (result == GL_INVALID_VALUE) { LOG_ERROR("GPU_Program was not a value generated by OpenGL when trying to get location of uniform" << name); }
	else if (result == GL_INVALID_OPERATION) { LOG_ERROR("Invalid shader program provided when getting location of uniform " << name); }
	return result;
}


GLint GetUniformLocation(GLuint shaderProgramID, const GLchar* name) {
	GLint result = glGetUniformLocation(shaderProgramID, name);
	if (result == -1) { LOG_ERROR(name << " is not a valid uniform variable!"); }
	else if (result == GL_INVALID_INDEX) { LOG_ERROR(name << " is not a uniform variable(may have been optimized out by GLSL"); }
	else if (result == GL_INVALID_VALUE) { LOG_ERROR("GPU_Program was not a value generated by OpenGL when trying to get location of uniform" << name); }
	else if (result == GL_INVALID_OPERATION) { LOG_ERROR("Invalid shader program provided when getting location of uniform " << name); }
	return result;
}


bool ParseGLSLShader(const std::string& filename, std::string& outFile) {
	auto input = ASSET_DIR + filename;
	std::ifstream fileStream(ASSET_DIR + filename);
	if (!fileStream.is_open()) {
		LOG_ERROR("Failed to open shader file: " << filename);
		return false;
	}
	std::string line;
	while (getline(fileStream, line)) {
		//Check if the file has a include directive
		if (line.find("#") != std::string::npos) {
			if (line.find("include") != std::string::npos) {
				size_t begin = line.find("<") + 1;
				size_t end = line.find(">") - begin;
				std::string includeFilename = line.substr(begin, end);
				std::string includeFile;
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

GLuint CompileShader(const std::string& filename, GLenum shaderType) {
	std::ifstream stream(ASSET_DIR + filename);
	if (!stream.is_open()) {
		LOG_ERROR("Failed to open shader file: " << filename);
		return 0;
	}

	std::stringstream shaderSource;
	std::string line;
	while (getline(stream, line)) {
		//Check if the file has a include directive
		if (line.find("#") != std::string::npos) {
			if (line.find("include") != std::string::npos) {
				size_t begin = line.find("<") + 1;
				size_t end = line.find(">") - begin;
				std::string includeFilename = line.substr(begin, end);
				std::string includeFile;
				auto baseFilepath = filename.substr(0, filename.find_last_of('/'));
				baseFilepath.append("/");
				baseFilepath.append(includeFilename);
				ParseGLSLShader(baseFilepath, includeFile);
				shaderSource << includeFile;
				continue;	//Dont add the include line...
			}
		}
		shaderSource << line << "\n";
	}

	stream.close();

    return CompileShader(shaderSource.str().c_str(), shaderType);

#if 0
	GLuint shaderID = glCreateShader(shaderType);
	glShaderSource(shaderID, 1, (const char*)(shaderSource.str().c_str()), NULL);
	glCompileShader(shaderID);
	GLint success;
	GLchar infoLog[GLSL_LOG_SIZE];
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shaderID, GLSL_LOG_SIZE, NULL, infoLog);
		auto shaderName = (shaderType == GL_VERTEX_SHADER) ? "VertexShader" : "FragmentShader";
		LOG_ERROR(filename << shaderName << " compilation failed! \n" << infoLog);
		return 0;
	}
	return shaderID;
#endif
}

GLuint CompileShader(const char* source, GLenum shaderType) {
	GLuint shaderID = glCreateShader(shaderType);
	glShaderSource(shaderID, 1, &source, NULL);
	glCompileShader(shaderID);
	GLint success;
	GLchar infoLog[GLSL_LOG_SIZE];
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shaderID, GLSL_LOG_SIZE, NULL, infoLog);
		auto shaderName = (shaderType == GL_VERTEX_SHADER) ? "VertexShader" : "FragmentShader";
		LOG_ERROR(shaderName << " compilation failed! \n" << infoLog);
		return 0;
	}
	return shaderID;
}

GLuint LoadShaderFromSource(const char* vertexShaderSource, const char* fragmentShaderSource) {
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

GLuint LoadShaderFromSource(const char* vertexShaderSource, const char* fragmentShaderSource, const char* geomertyShaderSource) {
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

GLuint LinkShaders(GLuint vertexShaderID, GLuint fragmentShaderID) {
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

#if 1
GLuint LoadShaderFromFile(const std::string& vertexFilename, const std::string& fragmentFilename) {
	std::string vertexShaderSource, fragmentShaderSource;
	ParseGLSLShader(vertexFilename, vertexShaderSource);
	ParseGLSLShader(fragmentFilename, fragmentShaderSource);
	return LoadShaderFromSource(vertexShaderSource.c_str(), fragmentShaderSource.c_str());
}
#endif

#if 0
GLuint LoadShaderFromFile(const std::string& vertexFilename, const std::string& fragmentFilename) {
	GLuint vertexShaderID = CompileShader(vertexFilename, GL_VERTEX_SHADER);
	GLuint fragmentShaderID = CompileShader(fragmentFilename, GL_FRAGMENT_SHADER);
	return LinkShaders(vertexShaderID, fragmentShaderID);
}
#endif

GLuint LoadShaderFromFile(const std::string& vertexFilename, const std::string& fragmentFilename, const std::string& geometryFilename) {
	std::string vertexShaderSource, fragmentShaderSource, geometryShaderSource;
	ParseGLSLShader(vertexFilename, vertexShaderSource);
	ParseGLSLShader(fragmentFilename, fragmentShaderSource);
	ParseGLSLShader(geometryFilename, geometryShaderSource);
	return LoadShaderFromSource(vertexShaderSource.c_str(), fragmentShaderSource.c_str(), geometryShaderSource.c_str());
}
