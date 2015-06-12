#pragma once

#include<Universe\CelestialBody.h>
#include<Engine\IEntityManager.h>
#include<memory>
#include<Graphics\GL\GLSLProgram.h>
#include<Math\Random.h>

struct CelestialStandard{
	CelestialBodyType type;
	std::string tag;
	float mass;
	float radius;
};

class Universe {
public:
	std::vector<std::unique_ptr<CelestialBody>> bodies;

	double G = 6.67;
	int AU = 218140; 

	Universe(unsigned int seed);
	~Universe();

	std::unique_ptr<CelestialBody>& CreateCelestialBody(CelestialBodyType type, unsigned int seed);

	void Step(float deltaTime);
	void Render(float deltaTime);

private:
	unsigned int seed;
	Random rng;

	CelestialStandard standards[sizeof(CelestialBodyType)];
};

