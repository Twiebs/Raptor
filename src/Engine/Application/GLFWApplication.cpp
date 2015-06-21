#include "GLFWApplication.hpp"
GLFWApplication::GLFWApplication() {
}

GLFWApplication::~GLFWApplication() {
	glfwTerminate();
}

void GLFWApplication::InitGLEW() {
	glewExperimental = true;
	GLenum glewInitCode = glewInit();
	if (glewInitCode != GLEW_OK) {
		std::cout << "GLEW failed to initialize!";
		if (!glewIsSupported("GLEW_VERSION_4_5")) {
			std::cout << " GLEW Version not supported 4.0\n ";
		}
	}
}


double GLFWApplication::GetTime() {
	return glfwGetTime();
}

bool GLFWApplication::ShouldClose() {
	return shouldClose;
}


void GLFWApplication::BeginFrame() {
	glfwPollEvents();
}
void GLFWApplication::EndFrame() {
	glfwSwapBuffers(window);
}

void GLFWApplication::Create(std::string title, int width, int height, bool fullscreen) {
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
	window = glfwCreateWindow(width, height, title.c_str(), fullscreen ? primaryMonitor : nullptr, nullptr);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window!";
		glfwTerminate();
	}

	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_FALSE);
	glfwSetWindowPos(window, 400, 300);
	glfwSwapInterval(1);

	//Regiser GLFW callbacks



	InitGLEW();
}

void GLFWApplication::SetCursorHidden(bool isHidden) {
	glfwSetInputMode(window, GLFW_CURSOR, isHidden ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

void GLFWApplication::SetDisplayMode(DisplayMode mode) {
}

void GLFWApplication::RegisterInputService(InputService* input) {
	glfwSetWindowUserPointer(window, input);
	GLFWRegisterCallbacks(window);
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
	InputService* input = static_cast<InputService*>(glfwGetWindowUserPointer(window));
	input->FireKeyEvent(key, action, mods);
}

void GLFWApplication::GLFWMouseButtonCallback(GLFWwindow* window, int button, int action, int mods){
	InputService* input = static_cast<InputService*>(glfwGetWindowUserPointer(window));
	input->FireMouseButtonEvent(button, action, mods);
}

void GLFWApplication::GLFWCursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
	InputService* input = static_cast<InputService*>(glfwGetWindowUserPointer(window));
	input->FireCursorPosEvent(xpos, ypos);
}

void GLFWApplication::GLFWWindowCloseCallback(GLFWwindow* window) {
	InputService* input = static_cast<InputService*>(glfwGetWindowUserPointer(window));
	//TODO close behavior
}

void GLFWApplication::GLFWWindowSizeCallback(GLFWwindow* window, int width, int height) {
	InputService* input = static_cast<InputService*>(glfwGetWindowUserPointer(window));
}
#pragma endregion
