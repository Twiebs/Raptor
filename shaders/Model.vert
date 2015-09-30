#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 tangent;

out vec3 fragPos;
out vec3 fragNormal;
out vec2 fragTexCoord;
out mat3 tbn;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

void main() {
  vec3 bitangent = cross(tangent, normal);
  vec3 t = normalize(vec3(modelMatrix * vec4(tangent, 0.0)));
  vec3 b = normalize(vec3(modelMatrix * vec4(bitangent, 0.0)));
  vec3 n = normalize(vec3(modelMatrix * vec4(normal, 0.0)));
  tbn = mat3(t, b, n);

  fragTexCoord = texCoord;
  fragPos = vec3(modelMatrix * vec4(position, 1.0f));
  fragNormal = normal;
  gl_Position	= projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0f);
}
