#version 330
in vec2 vertexUV;
in vec4 vertexColor;

out vec4 fragColor;

uniform sampler2D sampler;
uniform bool isText = false;

void main() {
	vec4 textureColor = texture(sampler, vertexUV);
	vec4 color;

	if(isText)
		color = vec4(vertexColor.rgb, textureColor.r);
	else
		color = textureColor * vertexColor;

	fragColor = color;
}