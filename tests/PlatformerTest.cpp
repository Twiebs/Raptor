#include <Core/Platform.h>
#include <Box2D/Box2D.h>
#include <Graphics/Render2D.hpp>

#include <Engine/Physics2D.hpp>
#include <Graphics/imgui.h>
#include <Engine/GFX2D.hpp>
#include <Engine/ECSManager.hpp>

ECSManager manager(512);

enum COMPONENT_TYPE {
	COMPONENT_BODY,
	COMPONENT_SPRITE,
};

struct PhysicsBody2D {
	b2Body* body;
};


b2Body* playerBody;
Box2DRenderer* box2DRenderer;

GLuint nullTextureID;

#define TIME_STEP 1.0f/60.0f
#define VELOCITY_ITERATIONS 12
#define POSITION_ITERATIONS 12
b2World world(b2Vec2(0.0f, -9.8f));

b2Body* CreateBody() {
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;

    auto body = world.CreateBody(&bodyDef);

    b2FixtureDef fixDef;
    b2CircleShape shape;
    shape.m_radius = 0.5f;
    fixDef.shape = &shape;

    body->CreateFixture(&fixDef);
    return body;
}

b2Body* CreatePlatform (const Vector2& position, const Vector2& size) {
    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    bodyDef.position.x = position.x;
    bodyDef.position.y = position.y;
    auto body = world.CreateBody(&bodyDef);

    b2FixtureDef fixDef;
    b2PolygonShape shape;
    shape.SetAsBox(size.x * 0.5f, size.y * 0.5f);
    fixDef.shape = &shape;

    body->CreateFixture(&fixDef);
    return body;
}

#define ACCELERATION 20.0f
void PlatformerControls (b2Body* body) {
    F64 deltaTime = PlatformGetDeltaTime();
    const b2Vec2& velocity = body->GetLinearVelocity();
    float vx = velocity.x;
	float vy = velocity.y;

    if (PlatformGetKey(KEY_A)) {
        vx -= ACCELERATION * deltaTime;
    } else if (PlatformGetKey(KEY_D)) {
        vx += ACCELERATION * deltaTime;
    } else if (PlatformGetKey(KEY_W) || PlatformGetKey(KEY_SPACE)) {
		b2Vec2 force = b2Vec2(0.0f, 100.0f);
		body->ApplyForceToCenter(force, true);
	}

    body->SetLinearVelocity(b2Vec2(vx, vy));
}

void MainLoop (F64 deltaTime) {
    if (PlatformGetKey(KEY_ESCAPE)) {
        PlatformExit();
    }

    PlatformerControls(playerBody);

	world.Step(TIME_STEP, VELOCITY_ITERATIONS, POSITION_ITERATIONS);
	//world.DrawDebugData();

    static Camera2D camera(30.0f, 18.0f);
    camera.update();
    camera.position.x = playerBody->GetPosition().x;
    camera.position.y = playerBody->GetPosition().y;

	GFX2D::Begin();
	GFX2D::SetProjectionMatrix(camera.projectionView());
	manager.ProcessComponent(COMPONENT_SPRITE, [](const Entity& entity, void* spritePointer) {
		auto sprite = (Sprite*)spritePointer;
		auto body = (PhysicsBody2D*)manager.GetComponent(entity, COMPONENT_BODY);
		GFX2D::FillRect(body->body->GetPosition().x - 0.5f, body->body->GetPosition().y - 0.5f, 1.0f, 1.0f);
	});


	GFX2D::FillRect(playerBody->GetPosition().x - 0.5f, playerBody->GetPosition().y - 0.5f, 1.0f, 1.0f);
	GFX2D::Texture(nullTextureID, 0.0f, 0.0f, 1.0f, 1.0f);
	GFX2D::FillRect(1.0f, -2.0f, 3.0f, 3.0f, Color::RED);
	GFX2D::End();

	ImGui::BeginFrame();
	ImGui::Text("Test");
	ImGui::ShowTestWindow();
	ImGui::EndFrame();
}


int main() {
    PlatformCreate("Platfomer Test");
	GFX2D::Init();
	ImGui::Init();

	manager.RegisterComponentType(COMPONENT_BODY, sizeof(PhysicsBody2D), 512);
	manager.RegisterComponentType(COMPONENT_SPRITE, sizeof(Sprite), 512);
	manager.Init();

	for (U32 i = 0; i < 10; i++) {
		auto& entity = manager.CreateEntity();
		auto body = (PhysicsBody2D*)manager.CreateComponent(entity, COMPONENT_BODY);
		body->body = CreateBody();

		auto sprite = (Sprite*)manager.CreateComponent(entity, COMPONENT_SPRITE);
		sprite->width = 1.0f;
		sprite->height = 1.0f;
	}


    playerBody = CreateBody();
    CreatePlatform(Vector2(0.0f, -5.0f), Vector2(30.0f, 1.0f));

	nullTextureID = CreateTextureFromFile("assets/null.png");

    Box2DRenderer b2Renderer;
    box2DRenderer = &b2Renderer;
    world.SetDebugDraw(box2DRenderer);
	world.SetAllowSleeping(true);
	world.SetContinuousPhysics(true);
	world.SetSubStepping(true);

    PlatformRun(MainLoop);
}

