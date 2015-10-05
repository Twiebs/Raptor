#include "Lighting.hpp"
#include <cmath>

PointLight::PointLight(const Vector3& color) : color(color) {
	linear = 0.027f;
	quadratic = 0.0028f;
	float lightMax = std::fmaxf(std::fmaxf(color.x, color.y), color.z);
	radius = (-linear + sqrtf(linear * linear - 4 * quadratic * (1.0 - (256.0 / 5.0) * lightMax))) / (2 * quadratic);
}
