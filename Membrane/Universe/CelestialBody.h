#pragma once

#include<vector>

#include<Engine\Entity.h>

struct SolarSystemData {
	float ecliptic;
};

struct AtmosphericData {
	float standardPressure;
};

enum CelestialBodyType {
	STAR,
	TERRESTRIAL,
	JOVIAN,
	DWARF,
	MOON	
};


struct CelestialBodyData {
	CelestialBodyType type;
	std::string name;
	float mass;
	float radius;
	float albedo;

	float semiMajorAxis;
	float inclination;
	float eccentricity;

	float rotationPeriod;
	float axialTilt;
};

class CelestialBody : public Entity{
public:
	Vector3 linearVelocity;

	CelestialBodyData data;

	CelestialBody();
	~CelestialBody();
};