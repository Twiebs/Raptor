#include <Core/Engine.hpp>

#include "NoiseScene.h"
#include "ModelViewerScene.h"

#include <Platform\Platform.hpp>

int main() {
	//auto engine = std::make_unique<Engine>();
	//engine->Create("Raptor tools", 1280, 720, false);
	//engine->CreateScene<NoiseScene>();
	//engine->Run();

	PLATFORMInit("Raptor Tools", 1280, 720, false);

	while (true) {
		PLATFORMHandleInputEvents();
	}

	PLATFORMShutdown();
	return 0;
}
