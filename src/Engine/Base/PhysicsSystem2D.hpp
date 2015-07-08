#pragma once

#include <ECS/ISystem.hpp>
#include <Box2D/Box2D.h>
#include <Graphics/DEBUGRenderer.hpp>

#define PHYSICS_2D_TIME_STEP 1.0f/60.0f
#define PHYSICS_2D_POSITION_ITERATIONS 6
#define PHYSICS_2D_VELOCITY_ITERATIONS 6

class PhysicsRenderer2D : b2Draw {
public:
	PhysicsRenderer2D();
	virtual ~PhysicsRenderer2D();

	void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;
	void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;
	void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color) override;
	void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color) override;
	void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) override;
	void DrawTransform(const b2Transform& xf) override;

private:
	GLRenderGroup renderGroup;
};

class PhysicsSystem2D : public ISystem {
public:
	b2World* world;	//For now this will be a public var untill we wrap the b2World in here...
	virtual ~PhysicsSystem2D();

	bool Startup (ECSManager* manager) override;
	bool Shutdown (ECSManager* manager) override;
	void Update	(double deltaTime) override;

private:

};
