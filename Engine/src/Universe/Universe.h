#pragma once

#include<Universe\CelestialBody.h>
#include<Core\IEntityManager.h>
#include<memory>

struct CelestialStandard{
	CelestialBodyType type;
	float mass;
	float radius;
};

class Universe {
public:
	std::vector<std::unique_ptr<CelestialBody>> bodies;
	CelestialStandard SOLAR_STANDARD;
	CelestialStandard TERRESTRIAL_STANDARD;

	double G = 66.67;
	int AU = 10;

	Universe(unsigned int seed);
	~Universe();

	std::unique_ptr<CelestialBody>& CreateCelestialBody(CelestialBodyType type, unsigned int seed);

	void Step(float deltaTime);
private:
	unsigned int seed;

	Mesh* CreateCelestialMesh(CelestialBodyData* data, int resolution);
};

