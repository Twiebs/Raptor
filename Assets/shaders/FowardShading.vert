#version 450 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 tangent;

layout (location = 0) uniform mat4 model;
layout (location = 1) uniform mat4 view;
layout (location = 2) uniform mat4 projection;

out VertexOut {
    vec3 position;
    vec3 normal;
    vec2 texCoord;
    mat3 tbn;
} result;

void main() {
    //Calculate the Tangent, Bi-Tangent, Normal
    vec3 bitangent = cross(tangent, normal);
    vec3 t = normalize(vec3(model * vec4(tangent,   0.0)));
    vec3 b = normalize(vec3(model * vec4(bitangent, 0.0)));
    vec3 n = normalize(vec3(model * vec4(normal,    0.0)));
    result.tbn = mat3(t, b, n);

    result.position = vec3(model * vec4(position, 1.0));
    result.normal = transpose(inverse(mat3(model))) * normal;
    result.texCoord = texCoord;

    gl_Position = projection * view * model * vec4(position, 1.0f);
}
