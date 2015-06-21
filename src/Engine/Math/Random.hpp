#pragma once

#include <random>

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
