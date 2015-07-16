#include "IO.hpp"

#ifdef SDL
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

Pixmap* LoadPixmap(std::string filename) {
	SDL_Surface* image = IMG_Load(filename.c_str());
	if (image == nullptr) {
		LOG_ERROR("Could not open file: " << filename  << " :: "<< IMG_GetError());
        return nullptr;
	}

    Pixmap* pixmap = new Pixmap(image->w, image->h);
	memcpy(pixmap->data, image->pixels, image->w * image->h * 4);
	SDL_FreeSurface(image);
    return pixmap;
}


bool WritePixmap(Pixmap* pixmap, const std::string& filename) {
    SDL_Surface* image = SDL_CreateRGBSurface(0, pixmap->width, pixmap->height, 32, 0, 0, 0, 0);
    if(image == nullptr) {
        LOG_ERROR("Failed to allocate SDLSurface when writing pixmap to file!");
        return false;
    }

    memcpy(image->pixels, pixmap->data, pixmap->width * pixmap->height * 4);
    IMG_SavePNG(image, filename.c_str());
    SDL_FreeSurface(image);
    return true;
}

#endif



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
  GLchar infoLog[GLSL_LOG_SIZE];
  glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);

  if (!success) {
	  glGetShaderInfoLog(shaderID, GLSL_LOG_SIZE, NULL, infoLog);
    std::string shadeTypeName = (shaderType == GL_VERTEX_SHADER) ? ("VertexShader") : ("FragmentShader");
    LOG_ERROR(shadeTypeName << " compilation failed! \n" << infoLog);
    return 0;
  }
  return shaderID;
}

GLuint DEBUGLoadShaderFromSource(std::string vertexShaderSource, std::string fragmentShaderSource) {
  GLuint vertexShaderID = DEBUGCompileShader(vertexShaderSource, GL_VERTEX_SHADER);
  GLuint fragmentShaderID = DEBUGCompileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

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


GLuint DEBUGLoadShaderFromFile(const std::string& vertexFilename, const std::string& fragmentFilename) {
      std::string vertexShaderSource, fragmentShaderSource;
      ParseGLSLShader(vertexFilename, vertexShaderSource);
      ParseGLSLShader(fragmentFilename, fragmentShaderSource);
      return DEBUGLoadShaderFromSource(vertexShaderSource, fragmentShaderSource);
}

