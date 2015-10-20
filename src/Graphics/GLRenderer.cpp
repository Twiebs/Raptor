#include "GLRenderer.hpp"
#include <string>
#include <Graphics/GLSLProgram.hpp>


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