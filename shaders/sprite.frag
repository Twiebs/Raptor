#version 330
in vec2 vertUV;
in vec4 vertColor;

uniform sampler2D sampler;

out vec4 fragColor;

void main() {
	vec4 sampledColor = texture(sampler, vertUV);
	// fragColor = vec4(sampledColor.r, sampledColor.g, sampledColor.b, sampledColor.a);
	fragColor = vec4(1.0);
}
