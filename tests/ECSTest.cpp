
#include <Core/Platform.h>

#include <Engine/GFX2D.hpp>
#include <Engine/ECSManager.hpp>

#include <Math/Random.hpp>

ECSManager manager(512);

struct Sprite {
	float x, y;
	float w, h;
	U32 textureID;
};

enum ComponentTypes {
	COMPONENT_SPRITE,
};


void MainLoop (F64 deltaTime) {

	GFX2D::Begin();
	static auto totalTime = deltaTime;
	totalTime += deltaTime;
	manager.ProcessComponent(COMPONENT_SPRITE, [&](void* sprite_ptr) {
		auto sprite = (Sprite*)sprite_ptr;
		GFX2D::FillRect(sprite->x + (sin(totalTime)), sprite->y, sprite->w, sprite->h);
	});
	GFX2D::End();

}

int main() {
	PlatformCreate("ECSTest");
	GFX2D::Init();
	manager.RegisterComponentType(COMPONENT_SPRITE, sizeof(Sprite), 512);

	Random rng;
	for (U32 i = 0; i < 100; i++) {
		auto entity = manager.CreateEntity();
		auto sprite = (Sprite*)(manager.CreateComponent(entity, COMPONENT_SPRITE));


		sprite->x = rng.Range(-1.0f, 1.0f);
		sprite->y = rng.Range(-1.0f, 1.0f);
		sprite->w = 0.05;
		sprite->h = 0.09;
	}

	PlatformRun(MainLoop);
}
