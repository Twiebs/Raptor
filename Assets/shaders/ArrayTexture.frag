#version 330 core

in vec3 fragTexCoord;

uniform sampler2DArray sampler;

out vec4 fragColor;

void main () {
	fragColor = texture(sampler, fragTexCoord.xyz) + vec4(0.1);
}
