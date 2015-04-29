#include "CelestialFactory.h"

#include<glm\gtc\noise.hpp>

CelestialFactory::CelestialFactory(Universe* universe) {
	this->universe = universe;
}


CelestialFactory::~CelestialFactory() {
}

CelestialBody* CelestialFactory::CreateCelestialBody(CelestialBodyType type, unsigned int seed) {

	CelestialStandard* standard;
	switch (type) {
	case STAR: standard = &universe->SOLAR_STANDARD; break;
	default: standard = &universe->TERRESTRIAL_STANDARD; break;
	}

	CelestialBodyData* data = GenerateCelestialData(standard, seed);
	Mesh* mesh = CreateCelestialMesh(data, 32);

	CelestialBody* body = new CelestialBody(data);
	body->position.Set(data->semiMajorAxis, 0, 0);
	body->AddComponent(mesh);
	return body;
}

CelestialBodyData* CelestialFactory::GenerateCelestialData(CelestialStandard* standard, unsigned int seed) {
	CelestialBodyData* data = new CelestialBodyData();
	data->type = standard->type;
	data->mass = 1.0f * standard->mass;
	data->radius = 1.0f * standard->radius;
	data->albedo = 1.0f;
	if (standard->type == TERRESTRIAL) data->semiMajorAxis = 30 * universe->AU;
	return data;
}



Mesh* CelestialFactory::CreateCelestialMesh(CelestialBodyData* data, int resolution) {
	std::vector<Vertex> verticies;
	std::vector<unsigned int> indicies;

	for (int i = 0; i < 6; i++) {
		GeomertyUtils::BuildCubeFace((GeomertyUtils::CubeFace)i, resolution, verticies, indicies);
	}

	for (Vertex& vertex : verticies) {
		vertex.normal = Vector3(1);
		float featureSize = 12;
		vertex.position.Normalize();
		vertex.position = vertex.position * data->radius;
		float value = glm::simplex(glm::vec3(vertex.position.x / featureSize, vertex.position.y / featureSize, vertex.position.z / featureSize));
		vertex.position += (Vector3(2.0f) * value);
	}

	return new Mesh(verticies, indicies);
}
