#ifndef RAPTOR_LIGHTING_HPP
#define RAPTOR_LIGHTING_HPP

#include <Graphics/Color.hpp>
#include <Math/Vector3.hpp>
#include <GL/glew.h>

struct PointLight {
	Vector3 position;
	Vector3 color;
	float linear;
	float quadratic;
	float radius;
	PointLight(const Vector3& color);
};
//
//struct DirectionalLight {
//	Vector3 direction = Vector3(0.0f, -1.0f, 0.0f);
//	Vector3 ambientColor = Vector3(0.1f, 0.1f, 0.1f);
//	Vector3 diffuseColor = Vector3(0.5f, 0.5f, 0.5f);
//	Vector3 specularColor = Vector3(0.5f, 0.5f, 0.5f);
//};


struct DirectionalLight {
	Vector3 direction = Vector3(0.0f, 1.0f, 0.0f);
	Vector3 color = Vector3(1.0f, 1.0f, 1.0f);
	float ambient = 0.15f;
};


void UniformDirectionalLight (GLuint shaderID, U32 lightIndex, const DirectionalLight& light);


#endif //RAPTOR_LIGHTING_HPP
