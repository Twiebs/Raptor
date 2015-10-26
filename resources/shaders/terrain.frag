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

struct DirectionalLight {
	vec3 direction;
	vec3 color;
	float ambient;
};

#define DIRECTIONAL_LIGHT_COUNT 1
uniform DirectionalLight directionalLights[DIRECTIONAL_LIGHT_COUNT];

layout (location = 3) uniform vec3 cameraViewPosition;

layout (binding = 0) uniform sampler2D diffuse_samplers[MATERIAL_COUNT];
layout (binding = MATERIAL_COUNT) uniform sampler2D specular_samplers[MATERIAL_COUNT];
layout (binding = MATERIAL_COUNT * 2) uniform sampler2D normal_samplers[MATERIAL_COUNT];
layout (binding = MATERIAL_COUNT * 3) uniform sampler2D alpha_samplers[MATERIAL_COUNT];

out vec4 outColor;

vec4 lerp (vec4 a, vec4 b, float s) {
	return vec4(a + (b - a) * s);
}


// vec3 get_normal (uint materialIndex) {
//     return fs_in.normal;
// }

vec3 get_normal(uint materialIndex) {
	vec3 normal;
	normal = texture(normal_samplers[materialIndex], fs_in.texCoord).rgb; // Get normal from normal_map
	normal = normalize((normal * 2.0) - 1.0); // Map the Normal to [-1, 1]
	normal = normalize(fs_in.tbn * normal); // Transform the normal into worldSpace
	return normal;
}

vec3 apply_directional_light (DirectionalLight light, vec3 view_direction, vec3 diffuse_color, vec3 specular_color, vec3 normal) {
	float diffuse_factor = max(dot(normal, -light.direction), 0.0);

	vec3 halfway_dir = normalize(-light.direction + view_direction);
	float specular_factor = pow(max(dot(normal, halfway_dir), 0.0), 32);

	vec3 ambient_color = light.color * light.ambient * diffuse_color;
	diffuse_color *= light.color * diffuse_factor;
	specular_color *= light.color * specular_factor;
	return ambient_color + diffuse_color + specular_color;
}

vec3 apply_lighting (vec3 diffuse_color, vec3 specular_color, vec3 normal) {
		vec3 result;
		vec3 viewDir = normalize(cameraViewPosition - fs_in.position);


		for (int i = 0; i < DIRECTIONAL_LIGHT_COUNT; i++) {
			result += apply_directional_light(directionalLights[i], viewDir, diffuse_color, specular_color, normal);
		}
		return result;
}

struct DebugRenderSettings {
	bool disableNormalMaps;
};

uniform DebugRenderSettings debug_settings;

#define BlendOverlayf(base, blend) 	(base < 0.5 ? (2.0 * base * blend) : (1.0 - 2.0 * (1.0 - base) * (1.0 - blend)))
#define Blend(base, blend, funcf) 		vec4(funcf(base.r, blend.r), funcf(base.g, blend.g), funcf(base.b, blend.b), funcf(base.a, blend.a))
#define BlendOverlay(base, blend) 		Blend(base, blend, BlendOverlayf)

#define internal_overlay(a, b) (a < 0.5f ? (2.0 * a * b) : (1.0 - 2.0 * (1.0 - a) * 1.0 - b))
#define internal_blend(a, b, function) vec4(function(a.r, b.r), function(a.g, b.g), function(a.b, b.b))

#define overlay(a, b) internal_blend(a, b, internal_overlay)


void main () {
	vec3 baseColor = vec3(0.0);
	vec3 baseNormal = vec3(0.0);
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

	if (debug_settings.disableNormalMaps) {
		baseNormal = fs_in.normal;
	}

	vec3 finalColor = apply_lighting(baseColor, vec3(0.0), baseNormal);
	// if (debug_settings.showMaterialAlphaMap) {
	// 	outColor = texture(alpha_samplers[debug_settings.selectedMaterialIndex], vec2((fs_in.texCoord.x * TERRAIN_CELLS_PER_TEXCOORD) / TERRAIN_WIDTH, (fs_in.texCoord.y * TERRAIN_CELLS_PER_TEXCOORD) / TERRAIN_LENGTH));
	// } else {
	// 	outColor = vec4(finalColor, 1.0);
	// }

	vec3 skyColor = vec3(0.53, 0.77, 0.92);
	vec3 resultColor = mix(skyColor, finalColor, fs_in.visibility);
	outColor = vec4(resultColor, 1.0);
}
