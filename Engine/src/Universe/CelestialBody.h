#pragma once

#include<vector>

#include<Core\Entity.h>
#include<VoxelEngine\VoxelChunk.h>

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
	CelestialBodyData* data;
	std::vector<VoxelChunk*> chunks;

	void Render();

	CelestialBody(CelestialBodyData* data);
	~CelestialBody();
};