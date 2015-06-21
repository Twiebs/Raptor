#version 330 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 uv;

out vec2 fragUV;

uniform mat4 mvp;

void main()
{
	fragUV = uv;
    gl_Position	= mvp * vec4(position.xy, 0.0, 1.0);
} 