layout (binding = 0) uniform sampler2D positionBuffer;
layout (binding = 1) uniform sampler2D normalBuffer;
layout (binding = 2) uniform sampler2D colorBuffer;
layout (binding = 3) uniform samplerCube shadowMapSampler;
layout (binding = 4) uniform sampler2D ssaoSampler;

out vec4 outputColor;

#define DEBUG

#include <include/lighting.glsl>
#ifdef DEBUG
#include <include/debug.glsl>
#endif

void UnpackGBuffer (ivec2 texcoord, out FragmentInfo fragment) {
  fragment.position = texelFetch(positionBuffer, texcoord, 0).rgb;
  fragment.normal   = texelFetch(normalBuffer, texcoord, 0).rgb;
  fragment.color    = texelFetch(colorBuffer, texcoord, 0).rgb;
  fragment.specular = texelFetch(colorBuffer, texcoord, 0).a;
  fragment.depth    = texelFetch(positionBuffer, texcoord, 0).a;
}

void main() {
  FragmentInfo fragment;
  UnpackGBuffer(ivec2(gl_FragCoord.xy), fragment);

  switch(GDebug.drawMode) {
	case 0: {
        outputColor = ApplyLighting(fragment);
    } break;
	case 1: outputColor = vec4(fragment.position / 1024.0f, 1.0); break;
	case 2: outputColor = vec4((fragment.normal * 0.5) + vec3(0.5), 1.0); break;
	case 3: outputColor = vec4(fragment.color, 1.0); break;
	case 4: outputColor = vec4(vec3(fragment.specular), 1.0); break;
	case 5: {
       outputColor = vec4(vec3(fragment.depth), 1.0);
	} break;
    case 6: {
        ivec2 texcoord = ivec2(gl_FragCoord.xy);
        float ambientOcclusion = texelFetch(ssaoSampler, texcoord, 0).r;
        outputColor = vec4(vec3(ambientOcclusion), 1.0);
    } break;
  }
 }
