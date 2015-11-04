layout (location = 0) out vec4 positionBuffer;
layout (location = 1) out vec3 normalBuffer;
layout (location = 2) out vec4 colorBuffer;

#define SetPosition(p) positionBuffer.xyz = p
#define SetNormal(n) normalBuffer = n
#define SetColor(c) colorBuffer.rgb = c
#define SetSpecular(s) colorBuffer.a = s
#define SetDepth(d) positionBuffer.a = d

const float NEAR = 0.1;
const float FAR = 50.0;

float LinearizeDepth (float depth) {
    float z = (depth * 2.0) - 1.0;
    float result = (2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR));
    return result;
}
