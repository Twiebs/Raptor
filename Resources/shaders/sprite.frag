#version 330
in vec2 fragUV;

out vec4 fragColor;

uniform sampler2D sampler;

void main()
{
	vec4 texColor = texture(sampler, fragUV);
	vec4 vertColor = vec4(1.0, 1.0, 1.0, 1.0);
	fragColor = texColor;
}