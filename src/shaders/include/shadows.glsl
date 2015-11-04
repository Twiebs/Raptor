
vec3 ApplyPointLightShadows () {
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
