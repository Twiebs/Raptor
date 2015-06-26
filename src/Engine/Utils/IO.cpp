#include "IO.hpp"


bool ParseGLSLShader(const std::string& filename, std::string& outFile) {
      std::ifstream fileStream(filename);
      if(!fileStream.is_open()) {
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
                        ParseGLSLShader(includeFilename, includeFile);
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

//DEBUGstuff
GLuint DEBUGCompileShader(std::string& shaderSource, GLenum shaderType) {
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
    LOG_ERROR(shadeTypeName << " compilation failed! \n" << infoLog);
    return 0;
  }
  return shaderID;
}

GLSLProgram* DEBUGLoadShaderFromSource(std::string& vertexShaderSource, std::string& fragmentShaderSource) {
  GLuint vertexShaderID = DEBUGCompileShader(vertexShaderSource, GL_VERTEX_SHADER);
  GLuint fragmentShaderID = DEBUGCompileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

  GLuint programID = glCreateProgram();
  glAttachShader(programID, vertexShaderID);
  glAttachShader(programID, fragmentShaderID);
  glLinkProgram(programID);

  GLint success;
  GLchar infoLog[SHADER_INFO_LOG_SIZE];
  glGetProgramiv(programID, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(programID, SHADER_INFO_LOG_SIZE, NULL, infoLog);
    LOG_ERROR("SHADER PROGRAM LINK: " << infoLog);
    return nullptr;
  }
  glDetachShader(programID, vertexShaderID);
  glDetachShader(programID, fragmentShaderID);
  glDeleteShader(vertexShaderID);
  glDeleteShader(fragmentShaderID);

  return new GLSLProgram(programID);
}


GLSLProgram* DEBUGLoadShaderFromFile(const std::string& vertexFilename, const std::string& fragmentFilename) {
      std::string vertexShaderSource, fragmentShaderSource;
      ParseGLSLShader(vertexFilename, vertexShaderSource);
      ParseGLSLShader(fragmentFilename, fragmentShaderSource);

      return DEBUGLoadShaderFromSource(vertexShaderSource, fragmentShaderSource);
}

GLuint DEBUGLoadTexture(std::string filename) {
	Pixmap* pixmap = PlatformLoadPixmap(filename);

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //Disable byte-alignment restriction
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pixmap->width, pixmap->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixmap->data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	return textureID;
}