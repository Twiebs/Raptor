#include "Universe.h"

#include<Math\GeometryUtils.h>
#include<glm\gtc\noise.hpp>

Universe::Universe() {


}


Universe::~Universe() {

}

void Universe::SetSeed(unsigned int seed) {
	this->seed = seed;

	SOLAR_STANDARD = {
		STAR,
		1000.0f,
		128.0f
	};
	TERRESTRIAL_STANDARD = {
		TERRESTRIAL,
		1.0f,
		12.0f
	};
}

CelestialBody* Universe::CreateCelestialBody(CelestialBodyType type, unsigned int seed) {

	CelestialStandard* standard;
	switch (type) {
	case STAR: standard = &SOLAR_STANDARD; break;
	default: standard = &TERRESTRIAL_STANDARD; break;
	}

	CelestialBodyData* data = GenerateCelestialData(standard, seed);
	Mesh* mesh = CreateCelestialMesh(data, 32);

	CelestialBody* body = new CelestialBody(data);
	body->position.Set(data->semiMajorAxis, 0, 0);
	body->AddComponent(mesh);
	bodies.push_back(body);
	return body;
}

CelestialBodyData* Universe::GenerateCelestialData(CelestialStandard* standard, unsigned int seed) {
	CelestialBodyData* data = new CelestialBodyData();
	data->type = standard->type;
	data->mass = 1.0f * standard->mass;
	data->radius = 1.0f * standard->radius;
	data->albedo = 1.0f;
	if (standard->type == TERRESTRIAL) data->semiMajorAxis = 30 * AU;
	return data;
}



Mesh* Universe::CreateCelestialMesh(CelestialBodyData* data, int resolution) {
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


void Universe::Step(float deltaTime) {
	for (CelestialBody* source : bodies) {
		for (CelestialBody* target : bodies) {
			if (source == target) continue;

			Vector3 displacement = (target->position - source->position);
			float length = displacement.Length();
			float r2 = length * length;
			float totalMass = (source->data->mass + target->data->mass);
			
			float magnitude = (G * (totalMass / r2));
			Vector3 force(-magnitude, 0, 0);
			force = force * displacement.Normalize();
			Vector3 velocity = force / target->data->mass;
			target->position += (velocity * deltaTime);
		}
	}
}