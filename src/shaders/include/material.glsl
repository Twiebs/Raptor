// layout (binding = 0) uniform sampler2D diffuseSampler;
// layout (binding = 1) uniform sampler2D specularSampler;
// layout (binding = 2) uniform sampler2D normalSampler;
//
// vec3 GetNormal() {
//   vec3 result = texture(normalSampler, input.texCoord).xyz;
//   result = normalize(normal * 2.0 - 1.0);
//   result = normalize(input.tbn * normal);
//   normalBuffer = input.normal;
// }
