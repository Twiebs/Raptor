#include "GLFWApplication.h"
GLFWApplication::GLFWApplication() {
}

GLFWApplication::~GLFWApplication() {
	glfwTerminate();
}

void GLFWApplication::Attach(IApplicationStartable* startable) {
	this->startable = startable;
	startable->Start(this);
}

void GLFWApplication::Start() {
	running = true;
	static double lastTime = glfwGetTime();
	static double frameTime = glfwGetTime();

	startable->Start(this);
	while (running && !glfwWindowShouldClose(window)) {
		double currentTime = glfwGetTime();
		float deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		glfwPollEvents();
		startable->Update(deltaTime);
		glfwSwapBuffers(window);
	}
	Exit();
}

void GLFWApplication::Create(char* title, int width, int height, bool fullscreen) {
	this->width = width;
	this->height = height;

	int glfwInitCode = glfwInit();
	if (glfwInitCode != GL_TRUE) {
		std::cout << "Failed to initalize GLFW";
	}

	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
	window = glfwCreateWindow(width, height, title, fullscreen ? primaryMonitor : nullptr, nullptr);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window!";
		glfwTerminate();
	}

	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_FALSE);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetWindowPos(window, 400, 300);
	glfwSwapInterval(1);

	//Regiser GLFW callbacks
	glfwSetWindowUserPointer(window, this);
	GLFWRegisterCallbacks(window);

	InitGLEW();
}
void GLFWApplication::Exit() {
	running = false;
}

void GLFWApplication::SetCursorHidden(bool isHidden) {
	glfwSetInputMode(window, GLFW_CURSOR, isHidden ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

void GLFWApplication::SetDisplayMode(DisplayMode mode) {
}



#pragma region WindowFunctions
void GLFWApplication::Resize(int width, int height) {
	glfwSetWindowSize(window, width, height);
}
int GLFWApplication::GetWidth() const {
	return width;
}
int GLFWApplication::GetHeight() const {
	return height;
}
#pragma endregion

#pragma region Callbacks
void GLFWApplication::GLFWRegisterCallbacks(GLFWwindow* window) {
	glfwSetKeyCallback(window, GLFWKeyCallback);
	glfwSetMouseButtonCallback(window, GLFWMouseButtonCallback);
	glfwSetCursorPosCallback(window, GLFWCursorPosCallback);
	glfwSetWindowCloseCallback(window, GLFWWindowCloseCallback);
	glfwSetWindowSizeCallback(window, GLFWWindowSizeCallback);
}

void GLFWApplication::GLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	GLFWApplication* app = static_cast<GLFWApplication*>(glfwGetWindowUserPointer(window));
	app->FireKeyEvent(key, action, mods);
}

void GLFWApplication::GLFWMouseButtonCallback(GLFWwindow* window, int button, int action, int mods){
	GLFWApplication* app = static_cast<GLFWApplication*>(glfwGetWindowUserPointer(window));
	app->FireMouseButtonEvent(button, action, mods);
}

void GLFWApplication::GLFWCursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
	GLFWApplication* app = static_cast<GLFWApplication*>(glfwGetWindowUserPointer(window));
	app->FireCursorPosEvent(xpos, ypos);
}

void GLFWApplication::GLFWWindowCloseCallback(GLFWwindow* window) {
	GLFWApplication* app = static_cast<GLFWApplication*>(glfwGetWindowUserPointer(window));
	app->Exit();
}

void GLFWApplication::GLFWWindowSizeCallback(GLFWwindow* window, int width, int height) {

}
#pragma endregion