layout (location = 0) in vec3 in_position;

layout (location = 0) uniform mat4 uModelMatrix;
layout (location = 1) uniform mat4 uViewMatrix;
layout (location = 2) uniform mat4 uProjectionMatrix;

void main() {
    gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * vec4(in_position, 1.0f);
}
