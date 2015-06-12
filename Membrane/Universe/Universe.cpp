#include "Universe.h"

#include<Math\GeometryUtils.h>
#include<glm\gtc\noise.hpp>

#define SOLAR_RADIUS_MIN 0.086f //2MASS J0523-1403 -- Red Dwarf
#define SOLAR_RADIUS_MAX 1708.0f//UY Scuti -- Red super giant
#define SOLAR_MASS_MIN 0.08f	//Gliese 165B -- RedDwarf
#define SOLAR_MASS_MAX 256.0f	//R136a1 -- Wolf-Rayet star

//Redefine for debug
#define SOLAR_RADIUS_MAX 1.0f

Universe::Universe(unsigned int seed) {
	this->seed = seed;
	rng.Seed(seed);

	standards[STAR] = {
		STAR,
		"STAR",
		1000.0f * rng.Range(SOLAR_MASS_MIN, SOLAR_MASS_MAX),
		1024.0f * rng.Range(SOLAR_RADIUS_MIN, SOLAR_RADIUS_MAX)
	};
	standards[JOVIAN] = {
		JOVIAN,
		"JOVIAN",
		100,
		101.89f
	};
	standards[TERRESTRIAL] = {
		TERRESTRIAL,
		"TERRESTIRAL",
		1.0f,
		9.29f
	};
}


Universe::~Universe() {

}

std::unique_ptr<CelestialBody>& Universe::CreateCelestialBody(CelestialBodyType type, unsigned int seed) {
	bodies.push_back(std::make_unique<CelestialBody>());
	std::unique_ptr<CelestialBody>& body = bodies[bodies.size() - 1];

	//Figure out the standard that will be used to create the celestial body

	//This data reference is used throughout the function
	CelestialBodyData& data = body->data;
	CelestialStandard& standard = standards[type];
	data.type = standard.type;
	data.name = standard.tag;
	data.mass = 1.0f * standard.mass;
	data.radius = 1.0f * standard.radius;
	data.albedo = 1.0f;
	if (standard.type == TERRESTRIAL)
		data.semiMajorAxis = 0.003f * AU;
	else if (standard.type == JOVIAN)
		data.semiMajorAxis = 0.005f * AU;

	//Create the mesh
	std::vector<Vertex> verticies;
	std::vector<unsigned int> indicies;

	//Build a cube...
	for (int i = 0; i < 6; i++) {
		GeomertyUtils::BuildCubeFace((GeomertyUtils::CubeFace)i, 32, verticies, indicies);
	}

	for (Vertex& vertex : verticies) {
		float featureSize = 32;
		vertex.position.Normalize();
		vertex.uv.y = (vertex.position.y + 1.0f) / 2.0f;
		//vertex.position = vertex.position * data.radius;

		//Map UVS
		vertex.uv.x = vertex.position.x / (sqrt((vertex.position.x * vertex.position.x) + (vertex.position.y * vertex.position.y) + (vertex.position.z * vertex.position.z)));
		//vertex.uv.y = vertex.position.y / (sqrt((vertex.position.x * vertex.position.x) + (vertex.position.y * vertex.position.y) + (vertex.position.z * vertex.position.z)));

		//Make it a shpereoid if its terrestiral
		if (data.type == TERRESTRIAL) {
			float value = glm::simplex(glm::vec3(vertex.position.x / featureSize, vertex.position.y / featureSize, vertex.position.z / featureSize));
			vertex.position += (Vector3(1.0f) * value);
		}
	}

	//Do some stuff
	auto mesh = body->CreateComponent<Mesh>(verticies, indicies);
	mesh.CalculateNormals();
	body->position.Set(body->data.semiMajorAxis, 0, 0);
	return body;
}

void Universe::Step(float deltaTime) {

}
