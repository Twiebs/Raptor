#include <Core\Engine.hpp>

#include "NoiseScene.h"
#include "ModelViewerScene.h"

int main() {
	auto engine = std::make_unique<Engine>();
	engine->Create("Raptor tools", 1280, 720, false);
	engine->CreateScene<NoiseScene>();
	engine->Run();

	return 0;
}