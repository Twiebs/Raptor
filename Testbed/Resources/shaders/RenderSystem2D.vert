#version 330 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec4 color;

out vec2 vertexUV;
out vec4 vertexColor;

uniform mat4 mvp;

void main() {
	vertexUV = uv;
	vertexColor = color;
    gl_Position	= mvp * vec4(position.xy, 0.0, 1.0);
} 