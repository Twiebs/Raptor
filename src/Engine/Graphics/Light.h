#pragma once

#include <Math/Vector3.hpp>

struct Light {
	Vector3 color;

	float ambientIntensity;
	float diffuseIntensity;
	float specularIntensity;
};


struct DirectionalLight : Light {
	Vector3 direction;
};

struct PointLight : Light {
	Vector3 position;
};


class BaseLight {
public:
	Vector3 color;
	float ambient;

	BaseLight();
	~BaseLight();
};
