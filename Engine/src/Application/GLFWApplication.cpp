#include "GLFWApplication.h"
GLFWApplication::GLFWApplication() {



}

GLFWApplication::~GLFWApplication() {

}

void GLFWApplication::Start() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	running = true;
	static double lastTime = glfwGetTime();
	static double frameTime = glfwGetTime();

	while (running && engine->IsRunning() && !glfwWindowShouldClose(window)) {
		double currentTime = glfwGetTime();
		float deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		glfwPollEvents();
		engine->Update(deltaTime);
		glfwSwapBuffers(window);
	}
	Exit();
}

void GLFWApplication::Create(char* title, int width, int height, bool fullscreen) {
	engine = new Engine();
	window = InitGLFW(width, height, fullscreen);
	InitGLEW();
}



void GLFWApplication::Exit() {
	glfwTerminate();
}


#pragma region WindowFunctions
void GLFWApplication::Resize(int width, int height) {
	glfwSetWindowSize(window, width, height);
}
int GLFWApplication::GetWidth() {
	return width;
}
int GLFWApplication::GetHeight() {
	return height;
}
#pragma endregion

GLFWwindow* GLFWApplication::InitGLFW(unsigned int screenWidth, unsigned int screenHeight, bool fullscreen) {
	int glfwInitCode = glfwInit();
	if (glfwInitCode != GL_TRUE) {
		std::cout << "Failed to initalize GLFW";
		return nullptr;
	}

	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Engine", fullscreen ? primaryMonitor : nullptr, nullptr);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window!";
		glfwTerminate();
		return nullptr;
	}

	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_FALSE);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSwapInterval(1);

	//Regiser GLFW callbacks
	glfwSetWindowUserPointer(window, engine);
	GLFWRegisterCallbacks(window);
	return window;
}


void GLFWApplication::GLFWRegisterCallbacks(GLFWwindow* window) {
	glfwSetKeyCallback(window, GLFWKeyCallback);
	glfwSetCursorPosCallback(window, GLFWCursorPosCallback);
	glfwSetWindowCloseCallback(window, GLFWWindowCloseCallback);
	glfwSetWindowSizeCallback(window, GLFWWindowSizeCallback);
}
#pragma region Callbacks
void GLFWApplication::GLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));

	if (action == GLFW_PRESS) engine->inputProcessor.OnKeyDown(key, mods);
	else if (action == GLFW_RELEASE) engine->inputProcessor.OnKeyUp(key);
}

void GLFWApplication::GLFWCursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
	Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
	engine->inputProcessor.OnCursorPos(xpos, ypos);
}

void GLFWApplication::GLFWWindowCloseCallback(GLFWwindow* window) {
	Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
	engine->Exit();
}

void GLFWApplication::GLFWWindowSizeCallback(GLFWwindow* window, int width, int height) {

}
#pragma endregion