

#include <ECS/ECSManager.hpp>
#include <Graphics/DebugRenderer.hpp>

#if __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

#include <Platform/Platform.hpp>

ECSManager* manager;
DebugRenderer* renderer;

bool gRunning = false;

void MainLoop() {
	PlatformBeginFrame();
	gRunning = PlatformHandleInput();
	manager->Update(0.1f);
	renderer->Begin();
	renderer->PushRect(0.0f, 0.0f, 500.0f, 500.0f);
	renderer->End();
	PlatformEndFrame();
}

int main () {
  PlatformInit("Raptor Web", 1280, 720, false);
	gRunning = true;

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

	return 0;
}
