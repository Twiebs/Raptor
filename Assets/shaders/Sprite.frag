#version 330
in vec2 vertUV;
in vec4 vertColor;

uniform sampler2D sampler;

out vec4 fragColor;

void main() {
	fragColor = vec4(texture(sampler, vertUV).rgb, vertColor.a);
}
