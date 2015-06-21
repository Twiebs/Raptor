#pragma once

#include <iostream>
#include <Application/IApplication.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class GLFWApplication : public IApplication {
public:
	GLFWApplication();
	~GLFWApplication();

	void InitGLEW();

	void Create(std::string tile, int width, int height, bool fullscreen) override;

	void Resize(int width, int height) override;
	int GetWidth() const override;
	int GetHeight() const override;

	double GetTime() override;

	bool ShouldClose() override;

	void BeginFrame() override;
	void EndFrame() override;

	void SetDisplayMode(DisplayMode mode) override;
	void SetCursorHidden(bool isHidden) override;

	void RegisterInputService(InputService* input) override;


private:
	GLFWwindow* window;
	void GLFWRegisterCallbacks(GLFWwindow* window);
	static void GLFWKeyCallback(GLFWwindow* window, int button, int scancode, int action, int mods);
	static void GLFWMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void GLFWCursorPosCallback(GLFWwindow* window, double xpos, double ypos);
	static void GLFWScrollCallback(GLFWwindow* window, double deltaX, double deltaY);
	static void GLFWWindowSizeCallback(GLFWwindow* window, int width, int height);
	static void GLFWWindowCloseCallback(GLFWwindow* window);
};
