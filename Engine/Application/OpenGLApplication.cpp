
#include<Application\OpenGLApplication.h>


void __stdcall OpenGLApplication::GLDebugCallback(GLenum source, GLenum type, GLuint id,
	GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam) {
	std::cout << message;
}

OpenGLApplication::OpenGLApplication() {
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	//glDebugMessageCallback(GLDebugCallback, nullptr);
}

OpenGLApplication::~OpenGLApplication() {
}

int OpenGLApplication::InitGLEW() {
	glewExperimental = true;
	GLenum glewInitCode = glewInit();
	if (glewInitCode != GLEW_OK) {
		std::cout << "GLEW failed to initialize!";
		if (!glewIsSupported("GLEW_VERSION_4_5")) {
			std::cout << " GLEW Version not supported 4.0\n ";
		}
	}
	return glewInitCode;
}


