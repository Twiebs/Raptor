#include "LoadShaderTask.hpp"

LoadShaderTask::LoadShaderTask(AssetID id, AssetRegistry* registry, std::string vertSourceFile, std::string fragSourceFile) 
	: id(id), registry(registry), vertexShaderFilename(vertSourceFile), fragmentShaderFilename(fragSourceFile) { }

LoadShaderTask::~LoadShaderTask() {

}


void LoadShaderTask::Run(uint32 threadID) {

	std::function<bool(std::string&, std::string&)> processShaderFile = [&processShaderFile](std::string& filename, std::string& outFile) -> bool{
		std::ifstream fileStream(filename);

		if (fileStream.is_open()) {
			std::string line;
			while (getline(fileStream, line)) {
				//Check if the file has a include directive
				if (line.find("#") != std::string::npos) {
					if (line.find("include") != std::string::npos) {
						size_t begin = line.find("<") + 1;
						size_t end = line.find(">") - begin;
						std::string includeFilename = line.substr(begin, end);
						std::string includeFile;
						processShaderFile(includeFilename, includeFile);
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

		std::cout << "Error opening file: " << filename;	//XXX Error message
		return false;
	};

	processShaderFile(vertexShaderFilename, vertexShaderSource);
	processShaderFile(fragmentShaderFilename, fragmentShaderSource);
}

void LoadShaderTask::Finalize(uint32 threadID) {
	auto compileShader = [](std::string& shaderSource, GLenum shaderType) -> GLuint {
		const GLchar* source = shaderSource.c_str();
		GLuint shaderID = glCreateShader(shaderType);
		glShaderSource(shaderID, 1, &source, NULL);
		glCompileShader(shaderID);

		GLint success;
		GLchar infoLog[SHADER_INFO_LOG_SIZE];
		glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);

		if (!success) {
			glGetShaderInfoLog(shaderID, SHADER_INFO_LOG_SIZE, NULL, infoLog);
			std::string shadeTypeName = (shaderType == GL_VERTEX_SHADER) ? ("VertexShader") : ("FragmentShader");
			std::cout << "ERROR: " << shadeTypeName << " compilation failed! \n" << infoLog << std::endl;
			return 0;
		}
		return shaderID;
	};

	GLuint vertexShaderID = compileShader(vertexShaderSource, GL_VERTEX_SHADER);
	GLuint fragmentShaderID = compileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

	GLuint programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	GLint success;
	GLchar infoLog[SHADER_INFO_LOG_SIZE];
	glGetProgramiv(programID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(programID, SHADER_INFO_LOG_SIZE, NULL, infoLog);
		std::cout << "ERROR: SHADER PROGRAM LINK:: " << infoLog << std::endl;
	}
	glDetachShader(programID, vertexShaderID);
	glDetachShader(programID, fragmentShaderID);
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	registry->data[id - 1] = new GLSLProgram(programID);
}