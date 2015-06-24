
#include <GL/glew.h>
#include <Platform/Platform.hpp>
#include <Utils/IO.hpp>

#include <Core/Common.hpp>
#include <Graphics/DebugRenderer.hpp>

#include <ECS/ECSManager.hpp>
#include <Base/RenderSystem2D.hpp>

#if __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

const std::string vertexShaderSource =
"attribute vec4 position;"
"void main() {"
"gl_Position = position;"
"}";

const std::string fragmentShaderSource =
"void main() {"
"gl_FragColor = vec4(1.0, 0.0, 1.0, 1.0);"
"}";

GLuint vertexArrayID;
GLuint vertexBufferID;
GLuint elementBufferID;
GLSLProgram* shader;

ECSManager* ecs;

const float vertices[] {
	-0.5f, -0.5f,
	0.5f, -0.5f,
	0.5f, 0.5f,
	-0.5f, 0.5f,
};

const uint32 indices[] {
	0, 3, 2,
	0, 2, 1
};

bool running = false;


void FillRect(float32 x, float32 y, float32 width, float32 height) {
	float verts[] {
			x, y,
			x + width, y,
			x + width, y + height,
			x, y + height
	};

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 2 * 4, verts);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(vertexArrayID);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void MainLoop() {
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	static float scl = 0;
	scl += 0.05f;

	running = PlatformHandleInput();

	ecs->Update(0.1f);
	shader->Use();
	FillRect(-0.5f, -0.5f, 1.0f, 1.0f);
	FillRect(0.75f, 0.0f, 0.1f, 1.75f);
	PlatformEndFrame();
}

int main () {
	PlatformInit("Raptor GLTest", 1280, 720, false);
	running = true;
#ifdef __EMSCRIPTEN__
	LOG_INFO("THIS IS EMSCRIPTEN!");
#endif

	//Init the ECSManager
	ecs = new ECSManager();
	ecs->RegisterComponent<Transform2D>(0);
	ecs->RegisterComponent<SpriteComponent>(0);
	ecs->RegisterComponent<TextComponent>(0);
	ecs->Initalize();

	//TODO specifiy component registration params inside the systems?
	ecs->CreateSystem<RenderSystem2D>();

	auto entity = ecs->CreateEntity();


	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	glGenBuffers(1, &vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * 4, vertices, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &elementBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32) * 6, indices, GL_DYNAMIC_DRAW);

	glBindVertexArray(0);

	shader = DEBUGLoadShaderFromSource(vertexShaderSource, fragmentShaderSource);
#ifndef __EMSCRIPTEN__
	LOG_INFO("The main loop is running...");
	while(running) {
		MainLoop();
	}
#else
	emscripten_set_main_loop(MainLoop, 60, 1);
#endif

}