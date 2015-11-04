#ifndef RAPTOR_LIGHTING_HPP
#define RAPTOR_LIGHTING_HPP

#include <Graphics/Color.hpp>
#include <Math/Vector3.hpp>
#include <GL/glew.h>

struct PointLight {
	Vector3 position = Vector3(0.0f, 4.0f, 0.0f);
	Vector3 color = Vector3(1.0f, 1.0f, 1.0f);
	float linear;
	float quadratic;
	PointLight();

	void SetRadius(float radius);
	inline float GetRadius() { return radius; }

private:
	float radius;
};


struct DirectionalLight {
	Vector3 direction = Vector3(0.0f, 1.0f, 0.0f);
	Vector3 color = Vector3(1.0f, 1.0f, 1.0f);
	float ambient = 0.15f;
};


void UniformDirectionalLight (GLuint shaderID, U32 lightIndex, const DirectionalLight& light);


#endif //RAPTOR_LIGHTING_HPP
