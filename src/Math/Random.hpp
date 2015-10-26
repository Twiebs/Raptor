#pragma once
#include <random>
#include <Core/types.h>

#include <Math/Vector2.hpp>
#include <Math/Geometry2D.hpp>

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

	int Range(int min, int max);
	float Range(float min, float max);
private:
	std::mt19937 rng;
};
