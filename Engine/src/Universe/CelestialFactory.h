#pragma once

#include<Graphics\Mesh.h>
#include<Math\GeometryUtils.h>
#include<Universe\CelestialBody.h>
#include<Universe\Universe.h>

class CelestialFactory {
public:
	CelestialFactory(Universe* universe);
	~CelestialFactory();

	CelestialBody* CreateCelestialBody(CelestialBodyType type, unsigned int seed);
	Mesh* CreateCelestialMesh(CelestialBodyData* data, int resolution);

	CelestialBodyData* GenerateCelestialData(CelestialStandard* standard, unsigned int seed);

private:
	Universe* universe;
};

