
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

std::string vertexShaderSource =
"attribute vec2 position;"
"attribute vec2 uv;"
"varying vec2 fragUV;"
"void main() {"
	"fragUV = uv;"
	"gl_Position = vec4(position.xy, 0.0f, 1.0f);"
"}";

std::string fragmentShaderSource =
"varying vec2 fragUV;"
"uniform sampler2D sampler;"
"void main() {"
	"gl_FragColor = texture2D(sampler, fragUV);"
"}";

GLuint vertexArrayID;
GLuint vertexBufferID;
GLuint elementBufferID;
GLuint textureID;
GLSLProgram* shader;

DebugRenderer* renderer;

ECSManager* ecs;
bool running = false;

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

struct Vert {
	Vector2 position;
	Vector2 uv;
};


Vert verticesWithStruct[4] {
	{ Vector2(-0.5f, -0.5f)/*, Vector2(0.0f, 0.0f)*/ },
	{ Vector2(0.5f, -0.5f)/*, Vector2(1.0f, 0.0f)*/ },
	{ Vector2(0.5f, 0.5f)/*, Vector2(1.0f, 1.0f) */},
	{ Vector2(-0.5f, 0.5f)/*, Vector2(0.0f, 1.0f) */}
};


void DEBUGDrawTexture(GLuint textureID, float x, float y, float width, float height) {
	Vector2 vectorVerts[] {
		Vector2(x, y),
			Vector2(0.0, 0.0),

			Vector2(x + width, y),
			Vector2(1.0, 0.0),

			Vector2(x + width, y + height),
			Vector2(1.0, 1.0),

			Vector2(x, y + height),
			Vector2(0.0, 1.0)
	};

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vert) * 4, (GLvoid*)vectorVerts);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(vertexArrayID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glBindVertexArray(vertexArrayID);
}


void DEBUGFillRectWithStruct(float x, float y, float width, float height) {
	Vector2 vectorVerts[] {
		Vector2(x, y),
		Vector2(0.0, 0.0),
		
		Vector2(x + width, y),
		Vector2(1.0, 0.0),
		
		Vector2(x + width, y + height),
		Vector2(1.0, 1.0),

		Vector2(x, y + height),
		Vector2(0.0, 1.0)
	};

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vert) * 4, (GLvoid*)vectorVerts);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(vertexArrayID);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void DEBUGInitRendererWithStruct() {
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	glGenBuffers(1, &vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vert) * 4, vertices, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vert), (GLvoid*)offsetof(Vert, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vert), (GLvoid*)offsetof(Vert, uv));
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &elementBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32) * 6, indices, GL_DYNAMIC_DRAW);

	glBindVertexArray(0);

	shader = DEBUGLoadShaderFromSource(vertexShaderSource, fragmentShaderSource);
}



void DEBUGFillRect(float32 x, float32 y, float32 width, float32 height) {
	float newVerts[] {
			x, y,
			x + width, y,
			x + width, y + height,
			x, y + height
	};

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 2 * 4, newVerts);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(vertexArrayID);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void DEBUGInitRenderer() {
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
}

void MainLoop () {
	running = PlatformHandleInput();

	static float x = -0.5f;
	static float dt = 0.0f;
	float xpos =  x * sin(dt);
	dt += 0.1f;

	PlatformBeginFrame();
	//shader->Use();
	//DEBUGDrawTexture(textureID, -0.5f, -0.5f, 1.0f, 1.0f);
	//DEBUGFillRectWithStruct(xpos, -0.5f, 1.0f, 1.0f);
	ecs->Update(0.1f);
	//Render Phase
	//DEBUGFillRect(0.75f, 0.0f, 0.1f, 1.75f);
	//renderer->Begin();
	//renderer->PushRect(-0.5f, -0.5f, 1.0f, 1.0f);
	//renderer->End();
	PlatformEndFrame();
}

int main () {
	PlatformInit("Raptor GLTest", 1280, 720, false);
	running = true;

	renderer = new DebugRenderer();

	//TODO specifiy component registration params inside the systems?
	ecs = new ECSManager();
	ecs->RegisterComponent<Transform2D>(0);
	ecs->RegisterComponent<SpriteComponent>(0);
	ecs->RegisterComponent<TextComponent>(0);
	ecs->Initalize();

	textureID = DEBUGLoadTexture("Assets/null.png");
	auto renderSystem = ecs->CreateSystem<RenderSystem2D>();
	//renderSystem->SetProjectionMatrix(Matrix4::Ortho(0, 1270, 0, 720, 0.1, 1000, 1.0));
	auto entity = ecs->CreateEntity();
	auto sprite =ecs->CreateComponent<SpriteComponent>(entity);
	sprite->x = 0;
	sprite->y = 0;
	sprite->width = 512;
	sprite->height = 512;
	sprite->textureID = textureID;

	//DEBUGInitRendererWithStruct();
	//DEBUGInitRendererWithVert();

#ifndef __EMSCRIPTEN__
	LOG_INFO("The main loop is running...");
	while(running) {
		MainLoop();
	}
#else
	emscripten_set_main_loop(MainLoop, 60, 1);
#endif

}
