#version 330
in vec2 vertUV;
in vec2 vertColor;

out vec4 fragColor;
uniform sampler2D sampler;

void main() {
	vec4 texColor = texture(sampler, vertUV);
	fragColor = texColor;
}
