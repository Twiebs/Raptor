in VertexOut {
	vec3 position;
	vec3 normal;
	vec2 texCoord;
	mat3 tbn;
} fs_in;

layout (binding = 0) uniform sampler2D diffuse_map;
layout (binding = 1) uniform sampler2D specular_map;
layout (binding = 2) uniform sampler2D normal_map;

struct DirectionalLight {
	vec3 direction;
	vec3 color;
	float ambient;
};

#define DIRECTIONAL_LIGHT_COUNT 1
uniform DirectionalLight directionalLights[DIRECTIONAL_LIGHT_COUNT];
layout (location = 3) uniform vec3 cameraViewPosition;


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

out vec4 outColor;

void main() {
	vec3 waterDiffuseColor = vec3(0.7, 0.8, 1.0);
	vec3 waterSpecularColor = vec3(1.0, 1.0, 1.0);
	vec3 finalColor = apply_lighting(waterDiffuseColor, waterSpecularColor, fs_in.normal);

  outColor = vec4(finalColor, 0.5);
}
