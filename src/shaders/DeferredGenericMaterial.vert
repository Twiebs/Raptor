layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_texcoord;
layout (location = 3) in vec3 in_tangent;

layout (location = 0) uniform mat4 uModelMatrix;
layout (location = 1) uniform mat4 uViewMatrix;
layout (location = 2) uniform mat4 uProjectionMatrix;

out FragmentData {
    vec3 position;
    vec3 normal;
    vec2 texcoord;
    mat3 tbn;
} fragment;


void CalculateTBNMatrix (in vec3 normal, out mat3 tbn) {
  vec3 bitangent = cross(normalize(in_tangent), normalize(in_normal));
  vec3 t = normalize(vec3(uModelMatrix * vec4(normalize(in_tangent),   0.0)));
  vec3 b = normalize(vec3(uModelMatrix * vec4(normalize(bitangent), 0.0)));
  vec3 n = normalize(vec3(uModelMatrix * vec4(normalize(in_normal),    0.0)));
  tbn = mat3(t, b, n);
}

void main() {
    vec4 world_position = uModelMatrix * vec4(in_position, 1.0f);
    vec4 view_position = uViewMatrix * world_position;
    fragment.position = world_position.xyz;
    fragment.texcoord = in_texcoord;

    fragment.normal = in_normal;
    CalculateTBNMatrix(in_normal, fragment.tbn);

    gl_Position = uProjectionMatrix * view_position;

    // mat3 normalMatrix = transpose(inverse(mat3(viewMatrix * modelMatrix)));
}
