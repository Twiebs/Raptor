#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

out vec3 fragPos;
out vec2 fragUV;

uniform mat4 mvp;

void main()
{
	fragPos = position;
    gl_Position	= mvp * vec4(position, 1.0f);
	fragUV = uv;
} 