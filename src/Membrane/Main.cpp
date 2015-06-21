#include<memory>

#include <Core/Engine.hpp>

#include "UICanvasTestScene.h"
#include "SpaceScene.h"
#include "UICanvasTestScene.h"
#include "LODScene.h"

int main() {
	auto engine = std::make_unique<Engine>();
	engine->CreateScene<SpaceScene>();
	engine->Run();

	return 0;
}