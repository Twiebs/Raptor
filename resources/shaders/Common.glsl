//This is a common file included by many of the differed renderering techinques!

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	sampler2D normal;
	float shininess;
};

uniform Material material;

struct PointLight {
  vec3 position;
  vec3 color;

  float linear;
  float quadratic;
	float radius;
};

uniform vec3 worldAmbientColor; //TODO

#define MAX_POINT_LIGHTS 64
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int pointLightCount;

#define SHADOW_BIAS 0.15
#define SHADOW_SAMPLES 20

#define ATTENUATION_LINEAR 0.027
#define ATTENUATION_CONSTANT 1.0
#define ATTENUATION_QUADRADIC 0.0028

uniform Configuration {
	bool useNormalMaps;
	bool castShadows;
} config;
