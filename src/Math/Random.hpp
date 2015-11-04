#pragma once
#include <random>
#include <Core/types.h>

#include <Math/Vector2.hpp>
#include <Math/Geometry2D.hpp>
#include <Math/Geometry3D.hpp>

float RandomInRange (float min, float max);
int RandomInRange (int min, int max);


//Generates random numbers
class Random {
public:
	Random();
	Random(int seed);
	~Random();

	void Seed(int seed);

	Vector2 PointInRectangle(const Rectangle& rect);
	V3 PointInCuboid(const Cuboid& cube);

	int Range(int min, int max);
	float Range(float min, float max);

	// This is a non-normalized vector with components
	// in the range [0.0, 1.0]
	V3 vector3();

private:
	std::mt19937 rng;
};
