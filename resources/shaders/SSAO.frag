#version 330 core
in vec2 fragTexCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D noiseMap;
uniform vec3 samples[64];

uniform mat4 projection;

//HACK This stuff needs to be dyanmic
const vec2 noiseScale = vec2(1280.0f / 4.0f, 720.0f / 4.0f);

out float fragColor;

float radius = 1.0;
void main() {
  //Get the stored values out of the GBuffer and SSAO noiseMap
  vec3 fragPos = texture(gPosition, fragTexCoord).xyz;
  vec3 normal = texture(gNormal, fragTexCoord).rgb;
  vec3 random = texture(noiseMap, fragTexCoord * noiseScale).xyz;

  // Calculate the tangnent, bitangnet, normal matrix_transform
  // For converting tangent-space samples to view-space
  vec3 tangent = normalize((random - normal) * dot(random, normal));
  vec3 bitangent = cross(normal, tangent);
  mat3 tbn = mat3(tangent, bitangent, normal);

  // Sample the Kernal and Calcuate the Occlusion Factor
  float occlusion = 0.0f;
  for (int i = 0; i < 64; i++) {
    vec3 sample = tbn * samples[i]; //Take the sample in tangent-space to view-space
    sample = (sample + fragPos) * radius; //Move the sample to the fragPos

    //The sample is transformed into clip-space to sample the depth value
    vec4 offset = projection * vec4(sample, 1.0);
    offset.xyz /= offset.w; //Perspective divide?
    offset.xyz = (offset.xyz * 0.5f) + 0.5f;  //Map to 0.0 - 1.0 for uv Coords

    //Now we obtain the depth out of the position buffer with the normalize offset
    float sampleDepth = -texture(gPosition, offset.xy).w;

    //Insure the depth values are close enough to be affected by SSAO
    float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
    occlusion += (sampleDepth >= sample.z ? 1.0 : 0.0) * rangeCheck;
  }

  //Normalize occlusion factor by the kernal size;
  occlusion = 1.0 - (occlusion / 64.0f);
  fragColor = occlusion;
}
