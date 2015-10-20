#pragma once

#include <Core/Common.hpp>
#include <random>

float RandomInRange (float min, float max);
int RandomInRange (int min, int max);


//Generates random numbers
class Random {
public:
	Random();
	Random(int seed);
	~Random();

	void Seed(int seed);

	int Range(int min, int max);
	float Range(float min, float max);
private:
	std::mt19937 rng;
};
