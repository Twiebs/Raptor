struct PointLight {
  vec3 position;
  vec3 color;

  float linear;
  float quadratic;
	float radius;
};

struct DirectionalLight {
	vec3 direction;
	vec3 color;
	float ambient;
};


// layout (location = 4) uniform float farPlane;

const float farPlane = 25.0;

#define MAX_DIRECTIONAL_LIGHT_COUNT 64
#define MAX_POINT_LIGHT_COUNT 64

const vec3 AMBIENT = vec3(0.1);


layout (location = 3) uniform vec3 cameraViewPosition;
layout (location = 4) uniform int directionalLightCount;
layout (location = 5) uniform int pointLightCount;

uniform DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHT_COUNT];
uniform PointLight pointLights[MAX_POINT_LIGHT_COUNT];


struct FragmentInfo {
  vec3 position;
  vec3 normal;
  vec3 color;
  float specular;
  float depth;
};

const float SHADOW_BIAS = 0.15;
const int SHADOW_SAMPLES = 20;
const vec3 DEPTH_SAMPLING_DISK[20] = vec3[] (
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1),
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);


vec3 ApplyDirectionalLight (DirectionalLight light, FragmentInfo fragment, vec3 viewDirection) {
  	float diffuseFactor = max(dot(fragment.normal, -light.direction), 0.0);

    vec3 halfwayDirection = normalize(-light.direction + viewDirection);
    float specularFactor = pow(max(dot(fragment.normal, halfwayDirection), 0.0), fragment.specular);

    vec3 ambientColor = light.color * light.ambient * fragment.color;
    vec3 diffuseColor = light.color * fragment.color * diffuseFactor;
    vec3 specularColor = light.color * fragment.color * specularFactor;
    return ambientColor + diffuseColor + specularColor;
}

vec3 ApplyPointLightViewSpace (in PointLight light, in FragmentInfo fragment) {
    vec3 lightDirection = normalize(light.position - fragment.position);
    float lightDistance = length(light.position - fragment.position);
    vec3 viewDirection = normalize(-fragment.position);
    vec3 halfwayDirection = normalize(lightDirection + viewDirection);

    float diffuseFactor = max(dot(lightDirection, fragment.normal), 0.0);
    float specularFactor = pow(max(dot(fragment.normal, halfwayDirection), 0.0), fragment.specular);
    float attenuationFactor = 1.0 / pow(((lightDistance/light.radius) + 1.0), 2);

    vec3 diffuseColor = light.color * fragment.color * diffuseFactor * attenuationFactor;
    vec3 specularColor = light.color * fragment.color * specularFactor * attenuationFactor;
    return diffuseColor + specularColor;
}

vec3 ApplyPointLightWorldSpace (PointLight light, FragmentInfo fragment) {
  vec3 lightDisplacement = light.position - fragment.position;
  float lightDistance = length(lightDisplacement);
  vec3 lightDirection = lightDisplacement / lightDistance; // Normalize the vector upfront since we have already got the distance

  float lightMax = max(max(light.color.r, light.color.g), light.color.b);
  float maxLightDistance = light.radius * (sqrt(lightMax / 0.0001) - 1);
  if (lightDistance > maxLightDistance)
    return vec3(0.0, 0.0, 0.0);

  // if (lightDistance > light.radius)
  //   return vec3(0.0, 0.0, 0.0);

  vec3 viewDirection = normalize(cameraViewPosition - fragment.position);
  vec3 halfwayDirection = normalize(lightDirection + viewDirection);

  float diffuseFactor = max(dot(lightDirection, fragment.normal), 0.0);
  float specularFactor = pow(max(dot(fragment.normal, halfwayDirection), 0.0), 16.0f);

  float attenuationFactor = 1.0 / pow(((lightDistance/light.radius) + 1.0), 2);

  vec3 diffuseColor = light.color * fragment.color * diffuseFactor * attenuationFactor;
  vec3 specularColor = light.color * fragment.color * specularFactor * attenuationFactor;
  return diffuseColor + specularColor;
}

vec4 DebugGetShadowDepthMapColor(in vec3 fragmentPosition, in vec3 lightPosition, in samplerCube shadowmap) {
  vec3 fragToLight = fragmentPosition - lightPosition;
  float closestDepth = texture(shadowmap, fragToLight).r;
  vec4 result = vec4(vec3(closestDepth), 1.0);
  return result;
}

float CalculateShadows (in vec3 fragmentPosition, in vec3 lightPosition, in samplerCube shadowmap) {
  vec3 fragToLight = fragmentPosition - lightPosition;

  float closestDepth = texture(shadowmap, fragToLight).r * farPlane;
  float currentDepth = length(fragToLight);

  const float bias = 0.05;
  float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
  return shadow;
}


// float CalculateShadows (in vec3 fragmentPosition, in vec3 lightPosition, in samplerCube shadowmap) {
//   float shadow = 0;
//   vec3 fragToLight = fragmentPosition - lightPosition;
// 	float currentDepth = length(fragToLight);
// 	float viewDistance = length(cameraViewPosition - fragmentPosition);
//
//     float diskRadius = (1.0 + (viewDistance / farPlane)) / 25.0;
//   	for(int i = 0; i < SHADOW_SAMPLES; i++) {
//   		float closestDepth = (texture(shadowmap, fragToLight + DEPTH_SAMPLING_DISK[i] * diskRadius).r) * farPlane;
//   		if(currentDepth - SHADOW_BIAS > closestDepth)
//   			shadow += 1.0;
//   	}
//   	shadow /= float(SHADOW_SAMPLES);
//     return shadow;
// }

//
// vec3 ApplyPointLightShadows (PointLight light) {
//   vec3 fragToLight = input.position - lightposition;
//   float currentDepth = length(fragToLight);
//   float viewDistance = length(viewPos - input.position);
//
//   float diskRadius = (1.0 + (viewDistance / farPlane)) / 25.0;
//   for(int i = 0; i < SHADOW_SAMPLES; i++) {
//     float closestDepth = (texture(depth_map, fragToLight + depthSamplingDisk[i] * diskRadius).r) * farPlane;
//     if(currentDepth - SHADOW_BIAS > closestDepth)
//       shadow += 1.0;
//   }
//   shadow /= float(SHADOW_SAMPLES);
// }




//
// vec3 LightFragment(FragmentInfo fragment) {
//   vec3 result = vec3(0.05); // TODO use world ambient!
//
//   for (int i = 0; i < pointLightCount; i++) {
//     vec3 lightDir = pointLights[i].position - fragment.position;
//     float lightDist = length(lightDir);
//     lightDir = normalize(lightDir);
//     vec3 viewDir = normalize(cameraViewPosition - fragment.position);
//     vec3 halfwayDir = normalize(lightDir + viewDir);
//
//     if (lightDist < pointLights[i].radius) {
//       vec3 norm = normalize(fragment.normal);
//       float diffuseFactor = max(dot(lightDir, norm), 0.0);
//
//       float specularFactor = pow(max(dot(norm, halfwayDir), 0.0), 16.0f);
//       float attenuation = 1.0f / (1.0 + pointLights[i].linear * lightDist + pointLights[i].quadratic * (lightDist * lightDist));
//
//       vec3 diffuseColor = pointLights[i].color * fragment.color * diffuseFactor * attenuation;
//       vec3 specularColor = pointLights[i].color * fragment.specular * specularFactor * attenuation;
//       result += vec3(diffuseColor + specularColor);
//     }
//
//   }
//   return result;
// }

vec4 ApplyLighting (FragmentInfo fragment) {

  vec3 color = vec3(0.0);
  vec3 viewDirection = normalize(cameraViewPosition - fragment.position);

  for (int i = 0; i < directionalLightCount; i++) {
    color += ApplyDirectionalLight(directionalLights[i], fragment, viewDirection);
  }


  for (int i = 0; i < pointLightCount; i++) {
    vec3 pointLightingResult = ApplyPointLightWorldSpace(pointLights[i], fragment);
   // pointLightingResult *= (1.0 - CalculateShadows(fragment.position, pointLights[i].position, shadowMapSampler));
    color += pointLightingResult;
  }

  ivec2 texcoord = ivec2(gl_FragCoord.xy);
  float ambientOcclusion = texelFetch(ssaoSampler, texcoord, 0).r;
  vec3 ambientColor = vec3(0.8) * fragment.color;
  color += ambientColor * ambientOcclusion;

  return vec4(color, 1.0);
}
