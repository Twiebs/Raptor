
#include<Application\OpenGLApplication.h>

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

