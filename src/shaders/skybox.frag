
in vec3 textureCoord;
layout (binding = 0) uniform samplerCube cubemap;


out vec4 outColor;
void main() {
  // outColor = vec4(1.0, 0.0, 0.0, 1.0);
  vec4 sampledDepth = texture(cubemap, textureCoord);
 //  outColor = vec4(vec3(sampledDepth.r), 1.0);
 outColor = vec4(1.0, 0.0, 1.0, 1.0);
}
