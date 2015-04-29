#pragma once

#include<Application\Application.h>
#include<GL\glew.h>

#include<iostream>

class OpenGLApplication : public IApplication {
public:
	int InitGLEW();
};
