#version 450 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 tangent;

layout (location = 0) uniform mat4 model;
layout (location = 1) uniform mat4 view;
layout (location = 2) uniform mat4 projection;

out VertexOut {
  vec3 worldPos;
  vec3 normal;
  vec3 tangent;
  mat3 tbn;
  vec2 texCoord;
} result;

void main() {
  vec3 bitangent = cross(normalize(tangent), normalize(normal));
  vec3 t = normalize(vec3(model * vec4(normalize(tangent),   0.0)));
  vec3 b = normalize(vec3(model * vec4(normalize(bitangent), 0.0)));
  vec3 n = normalize(vec3(model * vec4(normalize(normal),    0.0)));
  result.tbn = mat3(t, b, n);

  vec4 worldPos = model * vec4(position, 1.0f);
  result.worldPos = worldPos.xyz;

  result.normal = normal;
  result.tangent = tangent;
  result.texCoord = texCoord;

  gl_Position = projection * view * worldPos;
}
