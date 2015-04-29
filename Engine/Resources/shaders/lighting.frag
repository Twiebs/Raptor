#version 330 core

in vec3 fragPosition;
in vec3 fragNormal;

out vec4 color;

struct DirectionalLight { 
	vec3 direction;
	vec3 color;

	float ambientIntensity;
	float diffuseIntensity;
	float specularIntensity;
};

uniform vec3 viewPosition;
uniform vec3 lightPosition;
uniform vec3 objectColor;
uniform DirectionalLight directionalLight;
  
void main()
{
	//Ambient
	vec3 ambientColor = directionalLight.color * directionalLight.ambientIntensity;

	//Diffuse
	vec3 normal = normalize(fragNormal);
	vec3 lightDirection = normalize(lightPosition - fragPosition);
	float diffuseFactor = max(dot(normal, lightDirection), 0.0);
	vec3 diffuseColor = directionalLight.color * diffuseFactor;


	//Specular
	vec3 viewDirection = normalize(viewPosition - fragPosition);
	vec3 reflectDirection = reflect(-lightDirection, normal);
	float specularFactor = pow(max(dot(viewDirection, reflectDirection), 0.0), 32);
	vec3 specularColor = directionalLight.specularIntensity * specularFactor * directionalLight.color;

	vec3 result = (ambientColor + diffuseColor + specularColor) * objectColor;
	color = vec4(result, 1.0f);
}