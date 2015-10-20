// #define MATERIAL_COUNT X
// #define TERRAIN_WIDTH X
// #define TERRAIN_LENGTH x
// #define TERRAIN_CELLS_PER_TEXCOORD 4


in VertexOut {
	vec3 position;
	vec3 normal;
	vec2 texCoord;
	mat3 tbn;
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
// layout (binding = MATERIAL_COUNT) uniform sampler2D normal_samplers[MATERIAL_COUNT];
layout (binding = MATERIAL_COUNT) uniform sampler2D alpha_samplers[MATERIAL_COUNT];


//layout (binding = 0) uniform sampler2D diffuseSampler0;
//layout (binding = 1) uniform sampler2D diffuseSampler1;
//layout (binding = 2) uniform sampler2D diffuseSampler2;
//
//layout (binding = 3) uniform sampler2D alphaSampler0;
//layout (binding = 4) uniform sampler2D alphaSampler1;
//layout (binding = 5) uniform sampler2D alphaSampler2;

out vec4 outColor;

vec4 lerp (vec4 a, vec4 b, float s) {
	return vec4(a + (b - a) * s);
}

vec3 get_normal () {
    return fs_in.normal;
}

vec3 apply_directional_light (DirectionalLight light, vec3 view_direction, int materialIndex) {
	vec3 normal = get_normal();

	float diffuse_factor = max(dot(normal, -light.direction), 0.0);

	vec3 halfway_dir = normalize(-light.direction + view_direction);
	float specular_factor = pow(max(dot(normal, halfway_dir), 0.0), 32);

	vec3 ambient_color = light.color * light.ambient * vec3(texture(diffuse_samplers[materialIndex], fs_in.texCoord));
	vec3 diffuse_color = light.color * diffuse_factor * vec3(texture(diffuse_samplers[materialIndex], fs_in.texCoord));
	vec3 specular_color = light.color * specular_factor * vec3(texture(diffuse_samplers[materialIndex], fs_in.texCoord));
	return ambient_color + diffuse_color + specular_color;
}


vec3 apply_fragment_lighting (int materialIndex) {
	vec3 result;

	vec3 viewDir = normalize(cameraViewPosition - fs_in.position);
	for (int i = 0; i < DIRECTIONAL_LIGHT_COUNT; i++) {
		result += apply_directional_light(directionalLights[i], viewDir, materialIndex);
	}

//	for (int i = 0; i < DIRECTIONAL_LIGHT_COUNT; i++) {
//		vec3 normal = get_normal();
//		float diffuseFactor = max(dot(normal, directionalLights[i].direction), 0.0);
//		vec3 lightDir = directionalLights[i].direction;
//
//		vec3 halfwayDir = normalize(lightDir + viewDir);
//		float specularFactor = pow(max(dot(normal, halfwayDir), 0.0), 512);
//
//		vec3 ambientColor = directionalLights[i].ambient * vec3(texture(diffuseSampler0, fs_in.texCoord));
//		vec3 diffuseColor = directionalLights[i].diffuse * diffuseFactor * vec3(texture(diffuseSampler0, fs_in.texCoord));
//		vec3 specularColor = directionalLights[i].specular * specularFactor * vec3(texture(diffuseSampler0, fs_in.texCoord));
//		result += ambientColor + diffuseColor + specularColor;
//	}

	return result;
}

void main () {
//	vec4 diffuse_colors[MATERIAL_COUNT];
//	for (int i = 0; i < MATERIAL_COUNT; i++)
//		diffuse_colors[i] = texture(diffuse_samplers[i], fs_in.texCoord);



//	vec4 diffuseColor0 = texture(diffuseSampler0, fs_in.texCoord);
//	vec4 diffuseColor1 = texture(diffuseSampler1, fs_in.texCoord);
//	vec4 diffuseColor2 = texture(diffuseSampler2, fs_in.texCoord);

	vec4 color;

	vec4 colors[MATERIAL_COUNT];
	for (int i = 0; i < MATERIAL_COUNT; i++) {
		colors[i] = vec4(apply_fragment_lighting(i), 1.0);
		colors[i] *= texture(alpha_samplers[i], vec2(fs_in.position.x / TERRAIN_WIDTH, fs_in.position.z / TERRAIN_LENGTH)).r;
		color += colors[i];
	}


//	float alpha0 = texture(alphaSampler0, vec2(fs_in.position.x / TERRAIN_WIDTH, fs_in.position.z / TERRAIN_LENGTH)).r;
//	float alpha1 = texture(alphaSampler1, vec2(fs_in.position.x / TERRAIN_WIDTH, fs_in.position.z / TERRAIN_LENGTH)).r;
//	float alpha2 = texture(alphaSampler2, vec2(fs_in.position.x / TERRAIN_WIDTH, fs_in.position.z / TERRAIN_LENGTH)).r;
//
//	diffuseColor0 *= alpha0;
//	diffuseColor1 *= alpha1;
//	diffuseColor2 *= alpha2;

//	diffuseColor0 *= vec4(apply_fragment_lighting(), 1.0);
//	diffuseColor1 *= vec4(apply_fragment_lighting(), 1.0);
//	diffuseColor2 *= vec4(apply_fragment_lighting(), 1.0);

//	outColor = diffuseColor0 + diffuseColor1 + diffuseColor2;

	// outColor = color;
	// outColor = texture(diffuse_samplers[1], fs_in.texCoord);

	outColor = texture(alpha_samplers[1], vec2((fs_in.texCoord.x * TERRAIN_CELLS_PER_TEXCOORD) / TERRAIN_WIDTH, (fs_in.texCoord.y * TERRAIN_CELLS_PER_TEXCOORD) / TERRAIN_LENGTH));
}
