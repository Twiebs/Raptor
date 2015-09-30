#version 330 core
in vec3 fragPos;
in vec3 fragNorm;
in vec2 fragUV;
in float stormThreshold;

out vec4 color;

struct Light {
	vec3 position;
	vec3 color;
	float ambient;
	float diffuse;
	float specular;
};

//uniform vec3 viewPos;
uniform Light light;
uniform sampler1D colorGrad;

void main() {
	vec3 normal = normalize(fragNorm);

	vec3 ambient = light.color * light.ambient; 

	vec3 lightDir = normalize(light.position - fragPos);
	float diffuseFactor = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.color * diffuseFactor * light.diffuse;

	vec3 result = (ambient + diffuse);
	color = vec4(result * texture(colorGrad, fragUV.y).rgb, 1.0);
	//color += vec4(stormThreshold * 3.0, 0.0, 0.0, 0.0);
}