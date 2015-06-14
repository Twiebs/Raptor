#pragma once

#include<Application\OpenGLApplication.h>
#include<GLFW\glfw3.h>

class GLFWApplication : public OpenGLApplication{
public:
	GLFWApplication();
	~GLFWApplication();

	void Create(char* title, int width, int height, bool fullscreen) override;
	void Start() override;
	void Attach(IApplicationStartable* startable) override;
	void Exit() override;

	void Resize(int width, int height) override;
	int GetWidth() const override;
	int GetHeight() const override;

	void SetDisplayMode(DisplayMode mode) override;
	void SetCursorHidden(bool isHidden) override;

private:
	GLFWwindow* window;
	IApplicationStartable* startable;

	void GLFWRegisterCallbacks(GLFWwindow* window);
	static void GLFWKeyCallback(GLFWwindow* window, int button, int scancode, int action, int mods);
	static void GLFWMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void GLFWCursorPosCallback(GLFWwindow* window, double xpos, double ypos);
	static void GLFWScrollCallback(GLFWwindow* window, double deltaX, double deltaY);
	static void GLFWWindowSizeCallback(GLFWwindow* window, int width, int height);
	static void GLFWWindowCloseCallback(GLFWwindow* window);
};
