#version 330 core

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	sampler2D normal;
	float shininess;
};

struct LightColor {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct DirectionalLight {
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;

};

in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragTexCoord;
in vec4 fragLightSpacePos;
in mat3 tbn;

out vec4 outColor;

uniform Material material;

uniform PointLight pointLight;
uniform DirectionalLight directionalLight;
uniform samplerCube depthMap;
uniform float farPlane;

uniform int wireframeEnabled;
uniform vec3 viewPos;

uniform bool hasNormalMap;
uniform bool useBlin;

vec3 CalculateDirectionalLightColor(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 CalcuatePointLightColor(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
float CalculateShadows();

void main() {
	if(wireframeEnabled != 0) {
		outColor = vec4(1.0);
	} else {

		vec3 norm = vec3(0.0);
		if(hasNormalMap) {
			norm = vec3(texture(material.normal, fragTexCoord));
			norm = normalize(norm * 2.0 - 1.0);
			norm = normalize(tbn * norm);
		} else {
			norm = normalize(fragNormal);
		}

		vec3 viewDir = normalize(viewPos - fragPos);
		vec3 result = vec3(0.0);
		//result += CalculateDirectionalLightColor(directionalLight, norm, viewDir);
		result += CalcuatePointLightColor(pointLight, norm, fragPos, viewDir);
		outColor = vec4(result, 1.0);

		//Ambient
	// 	//float ambientStrength = 0.00f;
	// 	vec3 ambientColor = pointLight.ambient * vec3(texture(material.diffuse, fragTexCoord));
	// //	vec3 ambientColor = ambientStrength * light.color;
	//
	// 	//Diffuse
	// 	vec3 norm = normalize(fragNormal);
	// 	vec3 lightDir = normalize(pointLight.position - fragPos);
	// 	float diffuse = max(dot(norm, lightDir), 0.0);
	// 	vec3 diffuseColor = pointLight.diffuse * diffuse * vec3(texture(material.diffuse, fragTexCoord));
	// 	//vec3 diffuseColor = diffuse * light.color;
	//
	// 	// //Specular
	// 	float specularStrength = 0.5f;
	// 	 vec3 viewDir = normalize(viewPos - fragPos);
	// 	 vec3 reflectDir = reflect(-lightDir, norm);
	// 	 float specular = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	// 	// vec3 specularColor = light.specular * specular;
	// 	 vec3 specularColor = pointLight.specular * specular * vec3(texture(material.specular, fragTexCoord));
	// 	// vec3 specularColor =  specular * light.specular;
	// 	//vec3 specularColor = vec3(0.0);
	//
	// //	attenuation
	// 	float dist = length(pointLight.position - fragPos);
	// 	float attenuation = 1.0f / (atten.constant + atten.linear * dist + atten.quadratic * (dist * dist));
	// 	ambientColor *= attenuation;
	// 	diffuseColor *= attenuation;
	// 	specularColor *= attenuation;
	//
	// 	vec3 result = ambientColor + diffuseColor + specularColor;
	// 	outColor = vec4(result, 1.0);
	// }
	}
}

vec3 CalculateDirectionalLightColor(DirectionalLight light, vec3 normal, vec3 viewDir) {
	//Diffuse Factor
	vec3 lightDir = normalize(light.direction);
	float diffuseFactor = max(dot(normal, lightDir), 0.0);

  vec3 reflectDir = reflect(-lightDir, normal);
	float specularFactor = pow(max(dot(viewDir, reflectDir), 0.0), 32);

	vec3 ambientColor = light.ambient * vec3(texture(material.diffuse, fragTexCoord));
	vec3 diffuseColor = light.diffuse * diffuseFactor * vec3(texture(material.diffuse, fragTexCoord));
	vec3 specularColor = light.specular * specularFactor * vec3(texture(material.specular, fragTexCoord));
	return (ambientColor + diffuseColor + specularColor);
}

vec3 CalcuatePointLightColor(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
	//Diffuse Factor
	vec3 lightDir = normalize(light.position - fragPos);
	float diffuseFactor = max(dot(normal, lightDir), 0.0);

	//Specular Factor
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float specularFactor;
	if (useBlin) {
		specularFactor = pow(max(dot(normal, halfwayDir), 0.0), 32);
	} else {
		vec3 reflectDir = reflect(-lightDir, normal);
		specularFactor = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	}

	float shadowFactor = CalculateShadows();
	//float shadowFactor = 0;

	//Attenuation
	float dist = length(light.position - fragPos);
	float attenuationFactor = 1.0f / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

	vec3 ambientColor = light.ambient * vec3(texture(material.diffuse, fragTexCoord));
	vec3 diffuseColor = light.diffuse * diffuseFactor * vec3(texture(material.diffuse, fragTexCoord));
	vec3 specularColor = light.specular * specularFactor * vec3(texture(material.specular, fragTexCoord));
	ambientColor *= attenuationFactor;
	diffuseColor *= attenuationFactor;
	specularColor *= attenuationFactor;
	vec3 result = (ambientColor + (1.0 - shadowFactor) * (diffuseColor + specularColor));
	return result;
}

float CalculateShadows() {
	vec3 fragToLight = fragPos - pointLight.position;
	float closestDepth = texture(depthMap, fragToLight).r;
	closestDepth *= farPlane;
	float currentDepth = length(fragToLight);

	float bias = 0.05;
	float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
	//outColor = vec4(vec3(closestDepth / farPlane), 1.0);
	return shadow;

	//
	// vec3 projCoords = fragLightSpacePos.xyz / fragLightSpacePos.w;
	// projCoords = projCoords * 0.5 + 0.5;
	// if(projCoords.z > 1.0) {
	// 	return 0;
	// }
	//
	// float closestDepth = texture(shadowMap, projCoords.xy).r;
	// float currentDepth = projCoords.z;
	//
	// //float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
	// float bias = 0.05;
	// float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
	// return shadow;
}
