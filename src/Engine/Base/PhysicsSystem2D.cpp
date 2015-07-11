#include "PhysicsSystem2D.hpp"

PhysicsSystem2D::~PhysicsSystem2D() {

}
bool PhysicsSystem2D::Startup(ECSManager* manager) {
	world = new b2World(b2Vec2_zero);
	return true;
}

bool PhysicsSystem2D::Shutdown(ECSManager* manager) {
	delete world;
	return true;
}

void PhysicsSystem2D::Update(float64 deltaTime) {
	world->Step(PHYSICS_2D_TIME_STEP, PHYSICS_2D_VELOCITY_ITERATIONS, PHYSICS_2D_POSITION_ITERATIONS);
}
