#version 330 core
in vec3 fragPos;
in vec2 fragUV;

out vec4 color;

uniform sampler2D textureDiffuse1;

void main() {
	vec3 heightColor = vec3(0.0, 0.33, 0.01) + (vec3(0.66, -1.0, 0.66) * (fragPos.y / 128));
	color = vec4(heightColor, 1.0);
}