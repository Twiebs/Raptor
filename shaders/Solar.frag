#version 330 core
#include <Resources/shaders/noise3D.glsl>
in vec3 fragPos;

out vec4 color;

void main() {
	float n = (OctaveNoise(fragPos, 4, 40.0, 0.7) + 1.0) * 1;
	color = vec4(n, n, n, 1.0f);
}