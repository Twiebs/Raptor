#version 330 core
in vec2 fragUV;

out vec4 color;

struct Material{
	sampler2D textureDiffuse1;
	sampler2D textureSpecular1;
};

uniform Material material;

void main() {
	color = vec4(texture(material.textureDiffuse1, fragUV));
}