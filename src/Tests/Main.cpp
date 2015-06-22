#include <Platform/Platform.hpp>
#include <ECS/ECSManager.hpp>

#include <Graphics/DebugRenderer.hpp>

#if __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

ECSManager* manager;
DebugRenderer* renderer;

void MainLoop() {
	PlatformBeginFrame();
	manager->Update(0.1f);
	PlatformEndFrame();
}

int main () {
  PlatformInit("Raptor Web", 1280, 720, false);
	manager = new ECSManager();
	renderer = new DebugRenderer();
	//manager->CreateSystem<RenderSystem2D>();
	auto entity = manager->CreateEntity();

#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop(MainLoop, 60, 1);
#endif

	delete manager;
	delete renderer;
	return 0;
}
