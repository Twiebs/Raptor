#pragma once

#include<Application\Application.h>
#include<GL\glew.h>

#include<iostream>

class OpenGLApplication : public IApplication {
public:
	OpenGLApplication();
	~OpenGLApplication();

	int InitGLEW();
	
	//Itercepts OpenGL errors
	static void __stdcall GLDebugCallback(GLenum source, GLenum type, GLuint id,
		GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam);
};
