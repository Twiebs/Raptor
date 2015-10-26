#ifndef GL_ES
#define VERTEX_ATTRIBUTE(locationIndex) layout (location = locationIndex) in
#else
#define VERTEX_ATTRIBUTE(locationIndex) attribute
#endif

layout (location = 0) in vec3 position;

layout (location = 0) uniform mat4 model;
layout (location = 1) uniform mat4 view;
layout (location = 2) uniform mat4 projection;

void main() {
  gl_Position = projection * view * model * vec4(position, 1.0);
}
