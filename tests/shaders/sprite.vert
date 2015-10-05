#version 330 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec4 color;

out vec2 vertUV;
out vec4 vertColor;

uniform mat4 projectionView;

void main() {
	vertUV = uv;
	vertColor = color;
  	gl_Position	= vec4(position.xy, 0.0, 1.0);
}
