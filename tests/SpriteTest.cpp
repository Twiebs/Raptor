#include <Core/Platform.h>

#include <Engine/GFX2D.hpp>
#include <Math/Random.hpp>

void MainLoop(double deltaTime) {
	GFX2D::Begin();
	Random rng;
	for (U32 i = 0; i < 32; i++) {
		GFX2D::Rect(rng.Range(-1.0f, 1.0f), rng.Range(-1.0f, 1.0f), 0.05f, 0.05f);
	}



	GFX2D::End();
}

int main() {
    PlatformCreate("SpriteTest");
	GFX2D::Init();

    PlatformRun(MainLoop);
}