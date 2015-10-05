#ifndef RAPTOR_LIGHTING_HPP
#define RAPTOR_LIGHTING_HPP

#include <Graphics/Color.hpp>
#include <Math/Vector3.hpp>

struct PointLight {
	Vector3 position;
	Vector3 color;
	float linear;
	float quadratic;
	float radius;
	PointLight(const Vector3& color);
};



#endif //RAPTOR_LIGHTING_HPP
