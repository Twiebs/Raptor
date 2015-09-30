#version 450 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gColor;

layout (binding = 0) uniform sampler2D diffuse_map;
layout (binding = 1) uniform sampler2D specular_map;
layout (binding = 2) uniform sampler2D normal_map;

in VertexOut {
  vec3 worldPos;
  vec3 normal;
  vec3 tangent;
  mat3 tbn;
  vec2 texCoord;
} input;


void main () {
  gPosition = input.worldPos;
  //Normal is in range from [0, 1]
  vec3 normal = texture(normal_map, input.texCoord).xyz;
  normal = normalize(normal * 2.0 - 1.0); //Bring normal to [-1, 1]
  normal = normalize(input.tbn * normal);
  gNormal = input.normal;
  gColor.rgb = texture(diffuse_map, input.texCoord).rgb;
  gColor.a = texture(specular_map, input.texCoord).r;
}
