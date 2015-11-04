layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 tangent;

layout (location = 0) uniform mat4 modelMatrix;
layout (location = 1) uniform mat4 viewMatrix;
layout (location = 2) uniform mat4 projectionMatrix;

#define PI 3.14159265359
#include <include/liquid.glsl>

out VertexOut {
	vec3 position;
	vec3 normal;
	vec2 texCoord;
	mat3 tbn;
} vs_out;

#define GetNormal() GetWaveNormal(position.x, position.z)

void main() {
  vec3 bitangent = cross(tangent, normal);
  vec3 t = normalize(vec3(modelMatrix * vec4(tangent, 0.0)));
  vec3 b = normalize(vec3(modelMatrix * vec4(bitangent, 0.0)));
  vec3 n = normalize(vec3(modelMatrix * vec4(normal, 0.0)));
  vs_out.tbn = mat3(t, b, n);

	vec3 finalPosition = position;
	finalPosition.y += GetWaveHeight(finalPosition.x, finalPosition.z);

  vs_out.texCoord = texCoord;
  vs_out.position = vec3(modelMatrix * vec4(finalPosition, 1.0));
  vs_out.normal = GetNormal();
  gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(finalPosition, 1.0f);
}
