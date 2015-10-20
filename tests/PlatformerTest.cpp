#include <Core/Platform.h>
#include <Box2D/Box2D.h>
#include <Graphics/Render2D.hpp>

#include <Engine/Physics2D.hpp>
#include <Graphics/imgui.h>
#include <Engine/GFX2D.hpp>
#include <Engine/ECSManager.hpp>
#include <Math/Random.hpp>



enum COMPONENT_TYPE {
	COMPONENT_BODY,
	COMPONENT_TEXTURE,
	COMPONENT_TOTAL_COUNT
};

struct PhysicsBody2D {
	b2Body* body;
};

struct TextureComponent {
	U32 textureID;
};

Box2DRenderer* box2DRenderer;

b2Body* playerBody;

GLuint nullTextureID;

#define TIME_STEP 1.0f/60.0f
#define VELOCITY_ITERATIONS 12
#define POSITION_ITERATIONS 12
b2World world(b2Vec2(0.0f, -9.8f));


// Template-metafuck test
typedef TypeList<
		PhysicsBody2D,
		TextureComponent
> ComponentList;

ECSManager<ComponentList> manager(512);

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

struct ParticleEmitter2D {
	Array<Particle2D> particles;
	Vector2 position;
	Vector2 velocity;

	float fadeoutPeriod;
	float particlesPerSecond;

	float speed0, speed1;
	float theta0, theta1;

	float size0;
	float size1;

	float drag0;
	float drag1;

	float lifetime0;
	float lifetime1;

	float lifetime;
	float elapsedTime;

	Color color0;
	Color color1;
};


struct Particle2D {
	Vector2 position;
	Vector2 velocity;

	float size;
	float drag;
	Color color;

	float lifetime;
	float elapsedTime;
};

static void UpdateParticleEmitter2D (ParticleEmitter2D* emitter, float deltaTime) {
	static auto spawnParticle = [](ParticleEmitter2D* emitter) {
		auto particle = emitter->particles.CreateAndReturn();
		particle.position = emitter->position;
		particle.velocity = emitter->velocity;

		particle.size = Random::Range(emitter->size0, emitter->size1);
		particle.drag = Random::Range(emitter->drag0, emitter->drag1);
		particle.lifetime = Random::Range(emitter->lifetime0, emitter->lifetime1);

		auto colorLerpCoefficeant = Random::Range(0.0f, 1.0f);
		particle.color = Lerp(emitter->color0, emitter->color1, colorLerpCoefficeant);

		auto speed = Random::Range(emitter->speed0, emitter->speed1);
		auto theta = Random::Range(emitter->theta0, emitter->theta1);

		auto vx = speed * cos(theta);
		auto vy = speed * sin(theta);
		particle.velocity = Vector2(vx, vy);
	};

	static auto simulateParticle = [](Particle2D* particle, float deltaTime) {
		particle->velocity *= particle->drag;
		particle->elapsedTime += deltaTime;
	};

	for (U32 i = 0; i < emitter->particles.Count(); i++) {
		auto particle = &emitter->particles[i];
		simulateParticle(particle, deltaTime);
		if (particle->elapsedTime > particle->lifetime) {
			// Remove the particle here
		}
	}

	emitter->elapsedTime += deltaTime;
	if (emitter->elapsedTime > emitter->lifetime) {

	}
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

	manager.ProcessComponent<TextureComponent>([](const Entity& entity, TextureComponent* textureComponent) {
		auto physicsComponent = manager.GetComponent<PhysicsBody2D>(entity);
		const b2Vec2& position = physicsComponent->body->GetPosition();
		auto size = physicsComponent->body->GetFixtureList()->GetShape()->m_radius;
		GFX2D::Texture(textureComponent->textureID, position.x, position.y, size, size);
		// GFX2D::FillRect(body->body->GetPosition().x - 0.5f, body->body->GetPosition().y - 0.5f, 1.0f, 1.0f, textureComponent->textureID == 1 ? Color::WHITE : Color::RED);
	});


	GFX2D::End();


//	GFX2D::FillRect(playerBody->GetPosition().x - 0.5f, playerBody->GetPosition().y - 0.5f, 1.0f, 1.0f);
//	GFX2D::Texture(nullTextureID, 0.0f, 0.0f, 1.0f, 1.0f);
//	GFX2D::FillRect(1.0f, -2.0f, 3.0f, 3.0f, Color::RED);


	ImGui::BeginFrame();
	ImGui::Text("Test");
	ImGui::ShowTestWindow();
	ImGui::EndFrame();
}


typedef TypeList<PhysicsBody2D, TextureComponent> Test;

int main() {
    PlatformCreate("Platfomer Test");
	GFX2D::Init();
	ImGui::Init();

	Random rng;

	auto CreatePlatformerCharacter = [](const Vector2& position, U32 textureID) -> b2Body* {
		auto& entity = manager.CreateEntity();
		auto physicsBody = manager.CreateComponent<PhysicsBody2D>(entity);
		b2BodyDef bodyDef;
		bodyDef.position = b2Vec2(position.x, position.y);
		bodyDef.type = b2_dynamicBody;

		auto body = world.CreateBody(&bodyDef);

		b2FixtureDef fixDef;
		b2CircleShape shape;
		shape.m_radius = 0.5f;
		fixDef.shape = &shape;
		body->CreateFixture(&fixDef);
		physicsBody->body = body;

		auto textureComponent = manager.CreateComponent<TextureComponent>(entity);
		textureComponent->textureID = textureID;
		return body;
	};

	for (U32 i = 0; i < 10; i++) {
		CreatePlatformerCharacter(Vector2(rng.Range(-5.0f, 5.0f), rng.Range(-0.5f, 0.5f)), 0);
	}

	playerBody = CreatePlatformerCharacter(Vector2(0.0f, 0.0f), 1);
    CreatePlatform(Vector2(0.0f, -5.0f), Vector2(30.0f, 1.0f));

	nullTextureID = CreateTextureFromFile("assets/null.png");

    Box2DRenderer b2Renderer;
    box2DRenderer = &b2Renderer;
    world.SetDebugDraw(box2DRenderer);
	world.SetAllowSleeping(true);
//	world.SetContinuousPhysics(true);
//	world.SetSubStepping(true);

    PlatformRun(MainLoop);
}

