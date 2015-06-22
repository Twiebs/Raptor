// #include <Core/Engine.hpp>
//
// #include"TerrainScene.h"
// #include"ECSTestScene.h"

#include <Platform/Platform.hpp>
#include <ECS/EntityManager.hpp>
#include <Base/RenderSystem2D.hpp>
#include <Base/TextComponent.hpp>

EntityManager* manager;

void MainLoop() {
	PlatformBeginFrame();
	manager->Update(0.1f);
	PlatformEndFrame();
}

int main () {
  PlatformInit("Raptor Web", 1280, 720, false);
	manager = new EntityManager();
	//manager->CreateSystem<RenderSystem2D>();
	auto entity = manager->CreateEntity();
	manager->CreateSystem<RenderSystem2D>();
	auto text = manager->CreateComponent<TextComponent>(entity);


#ifdef __EMSCRIPTEN__
  //emscripten_set_main_loop(MainLoop, 60, 1);
#endif
  return 0;
}
