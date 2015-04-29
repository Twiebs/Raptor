#pragma once

#include<Universe\CelestialBody.h>

struct CelestialStandard {
	CelestialBodyType type;
	float mass;
	float radius;
};

class Universe {
public:
	//Celestial Standards
	CelestialStandard SOLAR_STANDARD;
	CelestialStandard TERRESTRIAL_STANDARD;

	double G = 66.67;
	int AU = 10;

	Universe(unsigned int seed);
	~Universe();

	CelestialBody* CreateCelestialBody(CelestialBodyType type, unsigned int seed);

	void Step(float deltaTime);


private:
	unsigned int seed;
	std::vector<CelestialBody*> bodies;

	CelestialBodyData* GenerateCelestialData(CelestialStandard* standard, unsigned int seed);
	Mesh* CreateCelestialMesh(CelestialBodyData* data, int resolution);
};

