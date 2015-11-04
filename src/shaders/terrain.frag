// #define MATERIAL_COUNT X
// #define TERRAIN_WIDTH X
// #define TERRAIN_LENGTH x
// #define TERRAIN_CELLS_PER_TEXCOORD 4

in VertexOut {
	vec3 position;
	vec3 normal;
	vec2 texCoord;
	mat3 tbn;
	float visibility;
} fs_in;

layout (binding = 0) uniform sampler2D diffuse_samplers[MATERIAL_COUNT];
layout (binding = MATERIAL_COUNT) uniform sampler2D specular_samplers[MATERIAL_COUNT];
layout (binding = MATERIAL_COUNT * 2) uniform sampler2D normal_samplers[MATERIAL_COUNT];
layout (binding = MATERIAL_COUNT * 3) uniform sampler2D alpha_samplers[MATERIAL_COUNT];

#define DEBUG
#include <include/deferred.glsl>
#ifdef DEBUG
#include <include/debug.glsl>
#endif


vec3 get_normal(uint materialIndex) {
	vec3 normal;
	normal = texture(normal_samplers[materialIndex], fs_in.texCoord).rgb; // Get normal from normal_map
	normal = normalize((normal * 2.0) - 1.0); // Map the Normal to [-1, 1]
	normal = normalize(fs_in.tbn * normal); // Transform the normal into worldSpace
	return normal;
}

void main () {

	vec3 baseColor;
	vec3 baseNormal;
	float baseSpecular;

	for (int i = 0; i < MATERIAL_COUNT; i++) {
		vec3 diffuse_color = texture(diffuse_samplers[i], fs_in.texCoord).rgb;
		vec3 normal = get_normal(i);

		float alpha = texture(alpha_samplers[i], vec2((fs_in.texCoord.x * TERRAIN_CELLS_PER_TEXCOORD) / TERRAIN_WIDTH, (fs_in.texCoord.y * TERRAIN_CELLS_PER_TEXCOORD) / TERRAIN_LENGTH)).r;
		diffuse_color *= alpha;
		diffuse_color += baseColor * (1.0 - alpha);
		normal *= alpha;
		normal += baseNormal * (1.0 - alpha);

		baseColor = diffuse_color;
		baseNormal = normal;
	}

	if (GDebug.disableNormalMaps) {
		baseNormal = fs_in.normal;
	}

//	vec3 skyColor = vec3(0.53, 0.77, 0.92);
//	vec3 resultColor = mix(skyColor, finalColor, fs_in.visibility);
//	outColor = vec4(resultColor, 1.0);

	SetPosition(fs_in.position);
	SetNormal(baseNormal);
	SetColor(baseColor);
	SetSpecular(32);
}
