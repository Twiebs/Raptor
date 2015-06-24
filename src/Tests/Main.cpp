#include <ECS/ECSManager.hpp>
#include <Graphics/DebugRenderer.hpp>
#include <Utils/IO.hpp>

#if __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

#include <Platform/Platform.hpp>


ECSManager* manager;
DebugRenderer* renderer;

bool gRunning = false;
GLuint gVertexArrayID;
GLuint gVertexBufferID;


void MainLoop() {
	PlatformBeginFrame();
	gRunning = PlatformHandleInput();
	//manager->Update(0.1f);
	//renderer->Begin();
	//renderer->PushRect(-0.75f, -0.75f, 1.5f, 1.5f);
	//renderer->End();
	renderer->DEBUGDrawRect(-0.5f, -0.5f, 1.0f, 1.0f);
//	renderer->DEBUGDrawRect();
	PlatformEndFrame();
}


int main () {
	PlatformInit("Raptor Web", 1280, 720, false);
	glViewport(0.0f, 0.0f, 1280.0f, 720.0f);
	gRunning = true;

	//Init the manager
	manager = new ECSManager();

	auto entity = manager->CreateEntity();

	renderer = new DebugRenderer();

#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(MainLoop, 60, 1);
#else

	while(gRunning) {
		MainLoop();
	}

#endif
	delete renderer;
	return 0;
}
