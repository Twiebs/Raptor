#ifndef RAPTOR_GLRENDERER_HPP
#define RAPTOR_GLRENDERER_HPP

#include <string>

#include <Math/Vector2.hpp>
#include <Math/Vector3.hpp>

#include <Graphics/Lighting.hpp>

#include <Core/types.h>
#include <GL/glew.h>

inline void BindTexture2DToUnit (GLuint textureID, U32 unitIndex);


inline void BindTexture2DToUnit (GLuint textureID, U32 unitIndex) {
	glActiveTexture(GL_TEXTURE0 + unitIndex);
	glBindTexture(GL_TEXTURE_2D, textureID);
}

namespace Uniform
{

inline GLint GetLocation(GLuint shaderID, const std::string& name);

inline void SetFloat(GLint location, float value);
inline void SetFloats(GLint startLocation, float* values, int count);

inline void SetVector2(GLint location, Vector2* value);
inline void SetVector2(GLint location, Vector2* values, int count);

inline void SetVector2(GLint location, Vector2* values, int count) {
	glUniform2fv(location, count, &values->x);
}

inline void SetVector3(GLint location, const Vector3& vector);

inline void SetDirectionalLight(DirectionalLight* light, U32 lightIndex, GLuint shaderID);

inline GLint GetLocation(GLuint shaderID, const std::string& name) {
	GLint location = glGetUniformLocation(shaderID, name.c_str());
	return location;
}

inline void SetFloats(GLint startLocation, float* values, int count) {
	glUniform1fv(startLocation, count, values);
}

inline void SetFloat(GLint location, float value) {
	glUniform1f(location, value);
}

inline void SetVector3(GLint location, const Vector3& vector) {
	glUniform3fv(location, 1, &vector.x);
}

inline void SetDirectionalLight(DirectionalLight* light, U32 lightIndex, GLuint shaderID) {
	GLint directionLoc = Uniform::GetLocation(shaderID, ("directionalLights[" + std::to_string(lightIndex) + "].direction").c_str());
	GLint colorLoc = Uniform::GetLocation(shaderID, ("directionalLights[" + std::to_string(lightIndex) + "].color").c_str());
	GLint ambientLoc = Uniform::GetLocation(shaderID, ("directionalLights[" + std::to_string(lightIndex) + "].ambient").c_str());

	Uniform::SetVector3(directionLoc, light->direction);
	Uniform::SetVector3(colorLoc, light->color);
	Uniform::SetFloat(ambientLoc, light->ambient);
}

}

void GLBasicTriangleTest();
void GLBasicQuadTest();
void GLIndexedQuadTest();

void GLCheckErrors();

#endif //RAPTOR_GLRENDERER_HPP