#include "Universe.h"

#include<Math\GeometryUtils.h>
#include<glm\gtc\noise.hpp>

Universe::Universe(unsigned int seed) {
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


Universe::~Universe() {

}

std::unique_ptr<CelestialBody>& Universe::CreateCelestialBody(CelestialBodyType type, unsigned int seed) {
	bodies.push_back(std::make_unique<CelestialBody>());
	std::unique_ptr<CelestialBody>& body = bodies[bodies.size() - 1];

	//Figure out the standard that will be used to create the celestial body
	CelestialStandard standard;
	if (type == STAR) 
		standard = SOLAR_STANDARD;
	if (type == TERRESTRIAL)
		standard = TERRESTRIAL_STANDARD;

	//This data reference is used throughout the function
	CelestialBodyData& data = body->data;
	data.type = standard.type;
	data.mass = 1.0f * standard.mass;
	data.radius = 1.0f * standard.radius;
	data.albedo = 1.0f;
	if (standard.type == TERRESTRIAL) 
		data.semiMajorAxis = 30 * AU;

	//Create the mesh
	std::vector<Vertex> verticies;
	std::vector<unsigned int> indicies;

	//Build a cube...
	for (int i = 0; i < 6; i++) {
		GeomertyUtils::BuildCubeFace((GeomertyUtils::CubeFace)i, 32, verticies, indicies);
	}

	for (Vertex& vertex : verticies) {
		vertex.normal = Vector3(1);
		float featureSize = 12;
		vertex.position.Normalize();
		vertex.position = vertex.position * data.radius;
		float value = glm::simplex(glm::vec3(vertex.position.x / featureSize, vertex.position.y / featureSize, vertex.position.z / featureSize));
		vertex.position += (Vector3(2.0f) * value);
	}

	//Do some stuff
	body->AddComponent(std::make_unique<Mesh>(verticies, indicies));
	body->position.Set(body->data.semiMajorAxis, 0, 0);
	return body;
}

void Universe::Step(float deltaTime) {
	for (std::unique_ptr<CelestialBody>& source : bodies) {
		for (std::unique_ptr<CelestialBody>& target : bodies) {
			if (source == target) continue;

			Vector3 displacement = (target->position - source->position);
			float length = displacement.Length();
			float r2 = length * length;
			float totalMass = (source->data.mass + target->data.mass);
			
			float magnitude = (G * (totalMass / r2));
			Vector3 force(-magnitude, 0, 0);
			force = force * displacement.Normalize();
			Vector3 velocity = force / target->data.mass;
			target->position += (velocity * deltaTime);
		}
	}
}