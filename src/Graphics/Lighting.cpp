#include "Lighting.hpp"
#include "GLSLProgram.hpp"
#include <cmath>

#include <GL/glew.h>

PointLight::PointLight(const Vector3& color) : color(color) {
	linear = 0.027f;
	quadratic = 0.0028f;
	float lightMax = std::fmaxf(std::fmaxf(color.x, color.y), color.z);
	radius = (-linear + sqrtf(linear * linear - 4 * quadratic * (1.0 - (256.0 / 5.0) * lightMax))) / (2 * quadratic);
}

inline void UniformVector3(GLint location, const Vector3& vector) {
	glUniform3fv(location, 1, &vector.x);
}

//void UniformDirectionalLight (GLuint shaderID, U32 lightIndex, const DirectionalLight& light) {
//	auto directionLoc = GetUniformLocation(shaderID, ("directionalLights[" + std::to_string(lightIndex) + "].direction").c_str());
//	auto ambientLoc = GetUniformLocation(shaderID, ("directionalLights[" + std::to_string(lightIndex) + "].ambient").c_str());
//	auto diffuseLoc = GetUniformLocation(shaderID, ("directionalLights[" + std::to_string(lightIndex) + "].diffuse").c_str());
//	auto specularLoc = GetUniformLocation(shaderID, ("directionalLights[" + std::to_string(lightIndex) + "].specular").c_str());
//
//	UniformVector3(directionLoc, light.direction);
//	UniformVector3(ambientLoc, light.ambientColor);
//	UniformVector3(diffuseLoc, light.diffuseColor);
//	UniformVector3(specularLoc, light.specularColor);
//}

void UniformDirectionalLight (GLuint shaderID, U32 lightIndex, const DirectionalLight& light) {
	auto directionLoc = GetUniformLocation(shaderID, ("directionalLights[" + std::to_string(lightIndex) + "].direction").c_str());
	auto colorLoc = GetUniformLocation(shaderID, ("directionalLights[" + std::to_string(lightIndex) + "].color").c_str());
	auto ambientLoc = GetUniformLocation(shaderID, ("directionalLights[" + std::to_string(lightIndex) + "].ambient").c_str());

	UniformVector3(directionLoc, light.direction);
	UniformVector3(colorLoc, light.color);
	glUniform1f(ambientLoc, light.ambient);
}

