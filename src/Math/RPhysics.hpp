#ifndef RAPTOR_PHYSICS_HPP
#define RAPTOR_PHYSICS_HPP

#include <Math/Vector3.hpp>

struct PhysicsState {
	Vector3 linearVelocity;
	Vector3 angularVelocity;
	float linearDamping;
	float angularDamping;
};

#endif