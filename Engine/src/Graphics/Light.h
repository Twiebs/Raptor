#pragma once

#include<Math\Vector3.h>

struct Light {
	Vector3 color;

	float ambientIntensity;
	float diffuseIntensity;
	float specularIntensity;
};


struct DirectionalLight : Light {
	Vector3 direction;
};


class BaseLight {
public:
	Vector3 color;
	float ambient;

	BaseLight();
	~BaseLight();
};

