#pragma once

#include <ECS/ISystem.hpp>
#include <Graphics/DEBUGRenderer.hpp>

#define PHYSICS_2D_TIME_STEP 1.0f/60.0f
#define PHYSICS_2D_POSITION_ITERATIONS 6
#define PHYSICS_2D_VELOCITY_ITERATIONS 6

class PhysicsSystem2D : public ISystem {
public:
	virtual ~PhysicsSystem2D();

	bool Startup (ECSManager* manager) override;
	bool Shutdown (ECSManager* manager) override;
	void Update	(double deltaTime) override;
private:


};
