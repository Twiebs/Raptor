#include <Core/Engine.hpp>

#include"TerrainScene.h"
#include"ECSTestScene.h"

int main() {
	auto engine = std::make_unique<Engine>();
	engine->Create("Raptor Tests", 1280, 720, false);
	engine->CreateScene<ECSTestScene>();
	engine->Run();

	return 0;
}