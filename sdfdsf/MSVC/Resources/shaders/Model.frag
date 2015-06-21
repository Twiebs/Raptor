#version 330 core
in vec2 fragUV;

out vec4 color;

uniform sampler2D textureDiffuse1;

void main() {
	color = vec4(texture(textureDiffuse1, fragUV));
}