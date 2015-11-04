const float NEAR = 0.1;
const float FAR = 1000.0f;

in FragmentData {
    vec3 position;
    vec3 normal;
    vec2 texcoord;
    mat3 tbn;
} fragment;

layout (binding = 0) uniform sampler2D uMaterialDiffuseSampler;
layout (binding = 1) uniform sampler2D uMaterialSpecularSampler;
layout (binding = 2) uniform sampler2D uMaterialNormalSampler;

layout (location = 4) uniform float uNearPlane;
layout (location = 5) uniform float uFarPlane;

layout (location = 0) out vec4 out_positionDepth;
layout (location = 1) out vec3 out_normal;
layout (location = 2) out vec4 out_albedoSpec;

float LinearizeDepth (float depth) {
    float z = depth * 2.0 - 1.0;
    return (2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR));
}

vec3 GetNormal () {
	vec3 result;
		result = texture(uMaterialNormalSampler, fragment.texcoord).rgb; // Get normal from normal_map
    if (result == vec3(0.0))
		return fragment.normal;
	result = normalize((result * 2.0) - 1.0); // Map the Normal to [-1, 1]
	result = normalize(fragment.tbn * result); // Transform the normal into worldSpace
	return result;
}

void main() {
    out_positionDepth.xyz = fragment.position;
    out_positionDepth.a = LinearizeDepth(gl_FragCoord.z);

    out_normal = GetNormal();

    vec3 diffuseColor = texture(uMaterialDiffuseSampler, fragment.texcoord).rgb;
    float specularExponent = texture(uMaterialSpecularSampler, fragment.texcoord).r;
    out_albedoSpec.rgb = diffuseColor;
    out_albedoSpec.a = specularExponent;
}
