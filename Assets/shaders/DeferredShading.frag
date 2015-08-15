#version 450 core
struct PointLight {
  vec3 position;
  vec3 color;

  float linear;
  float quadratic;
	float radius;
};

#define MAX_POINT_LIGHTS 64
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int pointLightCount;

layout (location = 3) uniform vec3 viewPos;
layout (location = 4) uniform vec3 worldAmbientColor; // TODO
layout (location = 5) uniform int draw_mode;

out vec4 outputColor;

layout (binding = 0) uniform sampler2D gPosition;
layout (binding = 1) uniform sampler2D gNormal;
layout (binding = 2) uniform sampler2D gColor;

struct FragmentInfo {
  vec3 position;
  vec3 normal;
  vec3 color;
  float specular;
};

void UnpackGBuffer(ivec2 texcoord, out FragmentInfo fragment) {
  fragment.position = texelFetch(gPosition, texcoord, 0).rgb;
  fragment.normal = texelFetch(gNormal, texcoord, 0).rgb;
  fragment.color = texelFetch(gColor, texcoord, 0).rgb;
  fragment.specular = texelFetch(gColor, texcoord, 0).a;
}

vec3 LightFragment(FragmentInfo fragment) {
  vec3 result = vec3(0.05); //TODO use world ambient!

  for (int i = 0; i < pointLightCount; i++) {
    vec3 lightDir = pointLights[i].position - fragment.position;
    float lightDist = length(lightDir);
    lightDir = normalize(lightDir);
    vec3 viewDir = normalize(viewPos - fragment.position);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    if (lightDist < pointLights[i].radius) {
      vec3 norm = normalize(fragment.normal);
      float diffuseFactor = max(dot(lightDir, norm), 0.0);

      float specularFactor = pow(max(dot(norm, halfwayDir), 0.0), 16.0f);
      float attenuation = 1.0f / (1.0 + pointLights[i].linear * lightDist + pointLights[i].quadratic * (lightDist * lightDist));

      vec3 diffuseColor = pointLights[i].color * fragment.color * diffuseFactor * attenuation;
      vec3 specularColor = pointLights[i].color * fragment.specular * specularFactor * attenuation;
      result += vec3(diffuseColor + specularColor);
    }

  }
  return result;
}

void main() {
  FragmentInfo fragment;
  UnpackGBuffer(ivec2(gl_FragCoord.xy), fragment);

  switch(draw_mode) {
    case 0: outputColor = vec4(LightFragment(fragment), 1.0); break;
    case 1: outputColor = vec4((fragment.position / 256.0f) + vec3(0.5, 0.5, 0.0), 1.0); break;
    case 2: outputColor = vec4((fragment.normal * 0.5) + vec3(0.5), 1.0); break;
    case 3: outputColor = vec4(fragment.color, 1.0); break;
    case 4: outputColor = vec4(vec3(fragment.specular), 1.0); break;
  }
  //outputColor = texture(gPosition, gl_FragCoord.xy);
 }
