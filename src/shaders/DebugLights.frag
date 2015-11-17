layout (location = 3) uniform vec3 lightColor;

out vec4 outColor;
void main() {
  outColor = vec4(lightColor, 1.0);
}
