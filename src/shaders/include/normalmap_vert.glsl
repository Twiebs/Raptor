
void CalculateTBNMatrix (out mat3 tbn) {
  vec3 bitangent = cross(normalize(tangent), normalize(normal));
  vec3 t = normalize(vec3(modelMatrix * vec4(normalize(tangent),   0.0)));
  vec3 b = normalize(vec3(modelMatrix * vec4(normalize(bitangent), 0.0)));
  vec3 n = normalize(vec3(modelMatrix * vec4(normalize(normal),    0.0)));
  tbn = mat3(t, b, n);
}
