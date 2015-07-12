#version 330 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec3 texCoord;

out vec3 fragTexCoord;

void main () {
	fragTexCoord = texCoord;
	gl_Position = vec4(position, 0.0, 0.0);
}
