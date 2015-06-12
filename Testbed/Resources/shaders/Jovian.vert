#version 330
#include <Resources/shaders/noise3D.glsl>

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

out vec3 fragPos;
out vec3 fragNorm;
out vec2 fragUV;
out float stormThreshold;

uniform mat4 model;
uniform mat4 mvp;

void main() {
	float s = 0.6;
	float t1 = snoise(normal * 2.0) - s;
	float t2 = snoise((normal + 800.0) * 2.0) - s;
	float t3 = snoise((normal + 1600.0) * 2.0) - s;
	float threshold = max(t1 * t2 * t3, 0.0);
	stormThreshold = threshold;

	float n1 = OctaveNoise(normal, 6, 10.0, 0.8) * 0.02f;
	float n2 = RidgedNoise(normal, 5, 5.8, 0.75) * 0.01;
	float n3 = snoise(normal * 0.1) * 0.001;
	float n = n1;

	fragPos	= vec3(model * vec4(position, 1.0));
	fragNorm = vec3(model * vec4(normal, 0.0));
	fragUV = uv;
	fragUV.y += n;
	gl_Position = mvp * vec4(position, 1.0f);
}