#version 450 core

out vec4 outputColor;
in VertexOut {
    vec3 position;
    vec3 normal;
    vec2 texCoord;
		mat3 tbn;
} input;

struct PointLight {
  vec3 position;
  vec3 color;

  float linear;
  float quadratic;
	float radius;
};

#define MAX_POINT_LIGHTS 64
#define ATTENUATION_CONSTANT 1.0
#define SHADOW_BIAS 0.15
#define SHADOW_SAMPLES 20
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform vec3 worldAmbientColor;
uniform int pointLightCount;

vec3 depthSamplingDisk[20] = vec3[] (
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1),
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);

layout (binding = 0) uniform sampler2D diffuse_map;
layout (binding = 1) uniform sampler2D specular_map;
layout (binding = 2) uniform sampler2D normal_map;
layout (binding = 3) uniform samplerCube depth_map;

layout (location = 3) uniform vec3 viewPos;
layout (location = 4) uniform float farPlane;

void main() {
	//Normal Mapping
	vec3 normal;
	if (false) {
		normal = texture(normal_map, input.texCoord).rgb; // Get normal from normal_map
		normal = normalize((normal * 2.0) - 1.0);           // Map the Normal to [-1, 1]
	  normal = normalize(input.tbn * normal);           // Transform the normal into worldSpace
	} else {
		normal = input.normal;
	}

  // Inital Colors and ambient
  vec3 ambientColor  = texture(diffuse_map, input.texCoord).rgb * vec3(0.05f); // WORLD AMBIENT
	vec3 diffuseColor  = texture(diffuse_map, input.texCoord).rgb * pointLights[0].color;
  vec3 specularColor = texture(specular_map, input.texCoord).rgb * pointLights[0].color;

  // Diffuse Factor
	vec3 lightDir = normalize(pointLights[0].position - input.position);
	diffuseColor *= max(dot(lightDir, normal), 0.0);

  // Specular Factor
	vec3 viewDir = normalize(viewPos - input.position);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	specularColor *= pow(max(dot(normal, halfwayDir), 0.0), 64.0);

  // Light Attenuation
	float distToLight = length(pointLights[0].position - input.position);
	float attenuationFactor = 1.0f / (ATTENUATION_CONSTANT + pointLights[0].quadratic * distToLight + pointLights[0].linear * (distToLight * distToLight));
  //World Ambient Lighting does not get attenuation
  diffuseColor *= attenuationFactor;
  specularColor *= attenuationFactor;

  //Shadow Mapping
  float shadow = 0;
  if (true) {
    vec3 fragToLight = input.position - pointLights[0].position;
  	float currentDepth = length(fragToLight);
  	float viewDistance = length(viewPos - input.position);

    float diskRadius = (1.0 + (viewDistance / farPlane)) / 25.0;
  	for(int i = 0; i < SHADOW_SAMPLES; i++) {
  		float closestDepth = (texture(depth_map, fragToLight + depthSamplingDisk[i] * diskRadius).r) * farPlane;
  		if(currentDepth - SHADOW_BIAS > closestDepth)
  			shadow += 1.0;
  	}
  	shadow /= float(SHADOW_SAMPLES);
  }

  vec3 resultColor = (ambientColor + (1.0 - shadow) * (diffuseColor + specularColor));
  outputColor = vec4(resultColor, 1.0f);
}
