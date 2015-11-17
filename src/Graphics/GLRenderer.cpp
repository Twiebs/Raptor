#include "GLRenderer.hpp"
#include <string>
#include <Graphics/GLSLProgram.hpp>

CREATE_LOG_CATEGORY(GLRenderer, LOGLEVEL_VERBOSE);


#define UNKNOWN_FUNCTION_STRING "UNKNOWN FUNCTION";
const char* GLDebugHelper::currentFunctionName = UNKNOWN_FUNCTION_STRING;
const char* GLDebugHelper::lastFunctionName = UNKNOWN_FUNCTION_STRING;

// TODO Asset Main Thread here...
GLDebugHelper::GLDebugHelper(const char* functionName) {
	lastFunctionName = currentFunctionName;
	currentFunctionName = functionName;
}

GLDebugHelper::~GLDebugHelper() {
	currentFunctionName = lastFunctionName;
	currentFunctionName = UNKNOWN_FUNCTION_STRING;
}

const char* GLDebugHelper::GetCurrentFunctionName() {
	return currentFunctionName;
}

void BindTexture2DToUnit(U32 textureUnit, U32 textureID) {
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_2D, textureID);
}


static const char* GetGLSourceDebugString (GLenum source) { 
	switch (source) {
	case GL_DEBUG_SOURCE_API: return "OpenGL API";
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "Window System";
	case GL_DEBUG_SOURCE_SHADER_COMPILER: 	return "ShaderCompiler";
	case GL_DEBUG_SOURCE_THIRD_PARTY: 	return "OpenGL ThirdParty";
	case GL_DEBUG_SOURCE_APPLICATION: 	return "RaptorInternal GLErrror";
	case GL_DEBUG_SOURCE_OTHER: return "OpenGL OtherError";
	default: return "Unknown GLSource";
	}
}

static const char* GetGLTypeDebugString(GLenum type) {
	switch(type) {
		case GL_DEBUG_TYPE_ERROR: return "ERROR";
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED"; 
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED"; 	
		case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY"; 	
		case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE"; 
		case GL_DEBUG_TYPE_MARKER: return "TypeMarker"; 	
		case GL_DEBUG_TYPE_PUSH_GROUP: return "GroupPushing"; 	
		case GL_DEBUG_TYPE_POP_GROUP: return "PopGroup"; 	
		case GL_DEBUG_TYPE_OTHER: return "Other";	
		default: return "UNKNOWN ERROR"; 
	}
}

static const char* GetGLSeverityString(GLenum severity) {
	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
	case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
	case GL_DEBUG_SEVERITY_LOW: return "LOW";
	case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
	default: return "RAPTOR_INTERNAL_ERROR_UNKNOWN SEVERITY!";
	}
}

static void __stdcall GLLogDebugMessage (GLenum source, GLenum type, GLuint id,
	GLenum severity, GLsizei length, const GLchar* message, const void* userParam) 
{
	const char* source_str = GetGLSourceDebugString(source);
	const char* type_str = GetGLTypeDebugString(type);
	const char* severity_str = GetGLSeverityString(severity);
	LOG_ERROR("[OpenGL] " << severity_str << " " << source_str << "::" << type_str  << "\n    " << message << "Error occured in durring a call in function: " << GLDebugHelper::GetCurrentFunctionName());
	if (severity == GL_DEBUG_SEVERITY_HIGH) {
		// assert(false);
	}
}

void EnableGLDebugMode() {
	glDebugMessageCallback(GLLogDebugMessage, nullptr);
}


const char* GLErrorToString(GLenum errorcode) {
	switch (errorcode) {
		case GL_INVALID_ENUM: 					return "GL_INVALID_ENUM";
		case GL_INVALID_VALUE: 					return "GL_INVALID_VALUE";
		case GL_INVALID_OPERATION: 				return "GL_INVALID_OPERATION";
		case GL_STACK_OVERFLOW: 				return "GL_STACK_OVERFLOW";
		case GL_STACK_UNDERFLOW: 				return "GL_STACK_UNDERFLOW";
		case GL_OUT_OF_MEMORY: 					return "GL_OUT_OF_MEMORY";
		case GL_INVALID_FRAMEBUFFER_OPERATION: 	return "GL_INVALID_FRAMEBUFFER_OPERATION";
		case GL_CONTEXT_LOST: 					return "GL_CONTEXT_LOST";
		default: 								return "UKNOWN GL_ERROR";
	}
}





void GLCheckErrors() {
	GLenum error = GL_NO_ERROR;
	while ((error = glGetError()) != GL_NO_ERROR) {
		printf("OPENGL ERROR: %s", GLErrorToString(error));
	}
}

void DEBUGRenderQuad() {
	static GLuint quadVAO = 0;
	static GLuint quadVBO = 0;
	if (quadVAO == 0) {
		GLfloat quadVertices[] = {
			// Positions        // Texture Coords
			-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// Setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}


void DEBUGRenderCube() {
	static GLuint cubeVAO = 0;
	static GLuint cubeVBO = 0;
	if (cubeVAO == 0) {
		GLfloat vertices[] = {
			// Back face
			-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // Bottom-left
			0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
			0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,  // top-right
			-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,  // bottom-left
			-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,// top-left

															  // Front face
			-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
			0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom-right 
			0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,  // top-right
			0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
			-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
			-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom-left
															   // Left face
			-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
			-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-left
			-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-left
			-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
			-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
			-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
															  // Right face
			0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
			0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
			0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-right         
			0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-right
			0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // top-left
			0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left     
															 // Bottom face
			-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
			0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // top-left
			0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,// bottom-left
			0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
			-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-right
			-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
																// Top face
			-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,// top-left
			0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
			0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top-right     
			0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
			-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,// top-left
			-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f // bottom-left        
		};
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		// Fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// Link vertex attributes
		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// Render Cube
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}



#if DEBUG || 1
void GLBasicTriangleTest() {
    static GLuint vertexBufferID, vertexArrayID;
    static GLuint shaderID;
    if (!vertexArrayID) {
        std::string vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 position;
        void main() {
            gl_Position = vec4(position.x, position.y, position.z, 1.0);
        }
    )";

        std::string fragmentShaderSource = R"(
        #version 330 core
        out vec4 color;
        void main() {
            color = vec4(1.0, 0.5, 0.2, 1.0);
        };
    )";

        shaderID = CreateShaderFromSource(vertexShaderSource.c_str(), fragmentShaderSource.c_str());

        GLfloat vertices[] = {
                -0.5f, -0.5f, 0.0f, // Left
                0.5f, -0.5f, 0.0f, // Right
                0.0f,  0.5f, 0.0f  // Top
        };

        glGenVertexArrays(1, &vertexArrayID);
        glBindVertexArray(vertexArrayID);

        glGenBuffers(1, &vertexBufferID);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }


    glUseProgram(shaderID);
    glBindVertexArray(vertexArrayID);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}

void GLBasicQuadTest() {
    static GLuint vertexArrayID, vertexBufferID, elementBufferID;
    static GLuint shaderProgramID;
    if (!vertexArrayID) {
        std::string vertexShaderSource = R"(
        attribute vec3 position;
        void main() {
            gl_Position = vec4(position, 1.0);
        };
    )";

        std::string fragmentShaderSource = R"(
        precision mediump float;
        void main() {
            gl_FragColor = vec4(1.0, 0.5, 0.2, 1.0);
        };
    )";

        GLfloat vertices[] = {
                -0.5f, -0.5f, 0.0f,
                0.5f, -0.5f, 0.0f,
                0.5f, 0.5f, 0.0f,

                0.5f, 0.5f, 0.0f,
                -0.5f, 0.5f, 0.0f,
                -0.5f, -0.5f, 0.0f,
        };

        GLuint indices[] = {
                0, 1, 2,
                2, 3, 0
        };

        shaderProgramID = CreateShaderFromSource(vertexShaderSource.c_str(), fragmentShaderSource.c_str());

        glGenVertexArrays(1, &vertexArrayID);
        glBindVertexArray(vertexArrayID);

        glGenBuffers(1, &vertexBufferID);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glGenBuffers(1, &elementBufferID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
    }

    glUseProgram(shaderProgramID);
    glBindVertexArray(vertexArrayID);
    //glDrawElements(GL_TRIANGLES, 6, GL_FLOAT, 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void GLIndexedQuadTest() {
    static GLuint vertexArrayID, vertexBufferID, elementBufferID;
    static GLuint shaderProgramID;
    if (!vertexArrayID) {
        std::string vertexShaderSource = R"(
    attribute vec3 position;
    void main() {
        gl_Position = vec4(position, 1.0);
    };
)";

        std::string fragmentShaderSource = R"(
    precision mediump float;
    void main() {
        gl_FragColor = vec4(1.0, 0.5, 0.2, 1.0);
    };
)";

        GLfloat vertices[] = {
                -0.5f, -0.5f, 0.0f,
                0.5f, -0.5f, 0.0f,
                0.5f, 0.5f, 0.0f,
                -0.5f, 0.5f, 0.0f,
        };

        GLuint indices[] = {
                0, 1, 2,
                2, 3, 0
        };

        shaderProgramID = CreateShaderFromSource(vertexShaderSource.c_str(), fragmentShaderSource.c_str());

        glGenVertexArrays(1, &vertexArrayID);
        glBindVertexArray(vertexArrayID);

        glGenBuffers(1, &vertexBufferID);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glGenBuffers(1, &elementBufferID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    glUseProgram(shaderProgramID);
    glBindVertexArray(vertexArrayID);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
#endif
