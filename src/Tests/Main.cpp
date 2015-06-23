

#include <ECS/ECSManager.hpp>
#include <Graphics/DebugRenderer.hpp>
#include <Utils/IO.hpp>

#if __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

#include <Platform/Platform.hpp>


const std::string vertexShaderSOurce =
  "attribute vec4 vPosition;    \n"
  "void main()                  \n"
  "{                            \n"
  "   gl_Position = vPosition;  \n"
  "}                            \n";

const std::string fragmentShaderSource =
  "precision mediump float;						\n"
  "void main()                                  \n"
  "{                                            \n"
  "  gl_FragColor = vec4 ( 0.0, 0.0, 1.0, 1.0 );\n"
  "}"
		;



ECSManager* manager;
DebugRenderer* renderer;

bool gRunning = false;
GLuint gVertexArrayID;
GLuint gVertexBufferID;

void MainLoop() {
	PlatformBeginFrame();
	gRunning = PlatformHandleInput();
	manager->Update(0.1f);
	renderer->Begin();
	renderer->PushRect(0.0f, 0.0f, 500.0f, 500.0f);
	renderer->End();
	PlatformEndFrame();
}

void Draw () {


	static GLuint programObject = DEBUGLoadShaderFromSource(vertexShaderSOurce, fragmentShaderSource)->GetProgram();
	GLfloat vVertices[] = {  0.0f,  0.5f, 0.0f,
                           -0.5f, -0.5f, 0.0f,
                            0.5f, -0.5f, 0.0f };

   // No clientside arrays, so do this in a webgl-friendly manner
   GLuint vertexPosObject;
   glGenBuffers(1, &vertexPosObject);
   glBindBuffer(GL_ARRAY_BUFFER, vertexPosObject);
   glBufferData(GL_ARRAY_BUFFER, 9*4, vVertices, GL_STATIC_DRAW);

   glViewport ( 0, 0, 1280, 720);
   glClear ( GL_COLOR_BUFFER_BIT );
   glUseProgram ( programObject );

   glBindBuffer(GL_ARRAY_BUFFER, vertexPosObject);
   glVertexAttribPointer(0 /* ? */, 3, GL_FLOAT, 0, 0, 0);
   glEnableVertexAttribArray(0);

   glDrawArrays ( GL_TRIANGLES, 0, 3 );
}

int main () {
  PlatformInit("Raptor Web", 1280, 720, false);
	gRunning = true;

	manager = new ECSManager();
	auto entity = manager->CreateEntity();

	renderer = new DebugRenderer();

#ifdef __EMSCRIPTEN__
//  emscripten_set_main_loop(MainLoop, 60, 1);
#else

	Draw();

//
//	while(gRunning) {
//		MainLoop();
//	}

#endif
//	delete renderer;
//	return 0;
}
