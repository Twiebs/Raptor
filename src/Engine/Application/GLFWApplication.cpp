#ifdef GLFW
#include "GLFWApplication.hpp"

#define KEY_SPACE = 32,
#define KEY_APOSTROPHE = 39,
#define KEY_COMMA = 44,
#define KEY_MINUS = 45,
#define KEY_PERIOD = 46,
#define KEY_SLASH = 47,
#define KEY_NUM_0 = 48,
#define KEY_NUM_1 = 49,
#define KEY_NUM_2 = 50,
#define KEY_NUM_3 = 51,
#define KEY_NUM_4 = 52,
#define KEY_NUM_5 = 53,
#define KEY_NUM_6 = 54,
#define KEY_NUM_7 = 55,
#define KEY_NUM_8 = 56,
#define KEY_NUM_9 = 57,
#define KEY_SEMICOLON = 59,
#define KEY_EQUAL = 61,
#define KEY_A = 65,
#define KEY_B = 66,
#define KEY_C = 67,
#define KEY_D = 68,
#define KEY_E = 69,
#define KEY_F = 70,
#define KEY_G = 71,
#define KEY_H = 72,
#define KEY_I = 73,
#define KEY_J = 74,
#define KEY_K = 75,
#define KEY_L = 76,
#define KEY_M = 77,
#define KEY_N = 78,
#define KEY_O = 79,
#define KEY_P = 80,
#define KEY_Q = 81,
#define KEY_R = 82,
#define KEY_S = 83,
#define KEY_T = 84,
#define KEY_U = 85,
#define KEY_V = 86,
#define KEY_W = 87,
#define KEY_X = 88,
#define KEY_Y = 89,
#define KEY_Z = 90,
#define KEY_LEFT_BRACKET = 91,
#define KEY_BACKSLASH = 92,
#define KEY_RIGHT_BRACKET = 93,
#define KEY_GRAVE_ACCENT = 96,
#define KEY_WORLD_1 = 161,
#define KEY_WORLD_2 = 162,

#define KEY_ESCAPE = 256,
#define KEY_ENTER = 257,
#define KEY_TAB = 258,
#define KEY_BACKSPACE = 259,
#define KEY_INSERT = 260,
#define KEY_DELETE = 261,
#define KEY_RIGHT = 262,
#define KEY_LEFT = 263,
#define KEY_DOWN = 264,
#define KEY_UP = 265,
#define KEY_PAGE_UP = 266,
#define KEY_PAGE_DOWN = 267,
#define KEY_HOME = 268,
#define KEY_END = 269,
#define KEY_CAPS_LOCK = 280,
#define KEY_SCROLL_LOCK = 281,
#define KEY_NUM_LOCK = 282,
#define KEY_PRINT_SCREEN = 283,
#define KEY_PAUSE = 284,
#define KEY_F1 = 290,
#define KEY_F2 = 291,
#define KEY_F3 = 292,
#define KEY_F4 = 293,
#define KEY_F5 = 294,
#define KEY_F6 = 295,
#define KEY_F7 = 296,
#define KEY_F8 = 297,
#define KEY_F9 = 298,
#define KEY_F10 = 299,
#define KEY_F11 = 300,
#define KEY_F12 = 301,
#define KEY_F13 = 302,
#define KEY_F14 = 303,
#define KEY_F15 = 304,
#define KEY_F16 = 305,
#define KEY_F17 = 306,
#define KEY_F18 = 307,
#define KEY_F19 = 308,
#define KEY_F20 = 309,
#define KEY_F21 = 310,
#define KEY_F22 = 311,
#define KEY_F23 = 312,
#define KEY_F24 = 313,
#define KEY_F25 = 314,
#define KEY_KP_0 = 320,
#define KEY_KP_1 = 321,
#define KEY_KP_2 = 322,
#define KEY_KP_3 = 323,
#define KEY_KP_4 = 324,
#define KEY_KP_5 = 325,
#define KEY_KP_6 = 326,
#define KEY_KP_7 = 327,
#define KEY_KP_8 = 328,
#define KEY_KP_9 = 329,
#define KEY_KP_DECIMAL = 330,
#define KEY_KP_DIVIDE = 331,
#define KEY_KP_MULTIPLY = 332,
#define KEY_KP_SUBTRACT = 333,
#define KEY_KP_ADD = 334,
#define KEY_KP_ENTER = 335,
#define KEY_KP_EQUAL = 336,
#define KEY_LEFT_SHIFT = 340,
#define KEY_LEFT_CONTROL = 341,
#define KEY_LEFT_ALT = 342,
#define KEY_LEFT_SUPER = 343,
#define KEY_RIGHT_SHIFT = 344,
#define KEY_RIGHT_CONTROL = 345,
#define KEY_RIGHT_ALT = 346,
#define KEY_RIGHT_SUPER = 347,
#define KEY_MENU = 348

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

#endif
