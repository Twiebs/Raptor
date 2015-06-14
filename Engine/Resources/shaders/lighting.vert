#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 model;
uniform mat4 mvp;

out vec3 fragNormal;
out vec3 fragPosition;

void main()
{
    gl_Position	= mvp * vec4(position, 1.0f);
	fragPosition = vec3(model * vec4(position, 1.0));
	fragNormal = vec3(model * vec4(normal, 0.0));
} 