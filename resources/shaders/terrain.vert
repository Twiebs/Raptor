layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 tangent;

layout (location = 0) uniform mat4 modelMatrix;
layout (location = 1) uniform mat4 viewMatrix;
layout (location = 2) uniform mat4 projectionMatrix;

layout (location = 3) uniform vec3 cameraViewPosition;

// Generated Shader Constants
const float FOG_DENSITY = 0.0015f;
const float FOG_GRADIENT = 2.0f;

out VertexOut {
	vec3 position;
	vec3 normal;
	vec2 texCoord;
	mat3 tbn;
	float visibility;
} vs_out;

void apply_atmospheric_fog() {
	float distanceToVertex = length(cameraViewPosition - position);
	vs_out.visibility = exp(-pow(distanceToVertex * FOG_DENSITY, FOG_GRADIENT));
}

void main() {
  vec3 bitangent = cross(tangent, normal);
  vec3 t = normalize(vec3(modelMatrix * vec4(tangent, 0.0)));
  vec3 b = normalize(vec3(modelMatrix * vec4(bitangent, 0.0)));
  vec3 n = normalize(vec3(modelMatrix * vec4(normal, 0.0)));
  vs_out.tbn = mat3(t, b, n);

	apply_atmospheric_fog();

  vs_out.texCoord = texCoord;
  vs_out.position = vec3(modelMatrix * vec4(position, 1.0f));
  vs_out.normal = normal;
  gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0f);
}
