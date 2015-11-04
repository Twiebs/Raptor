out float FragColor;
in vec2 fragment_texcoord;

layout(binding = 0) uniform sampler2D gPositionDepth;
layout(binding = 1) uniform sampler2D gNormal;
layout(binding = 2) uniform sampler2D texNoise;

layout (location = 1) uniform mat4 viewMatrix;
layout (location = 2) uniform mat4 projectionMatrix;
layout (location = 3) uniform int uKernelSize;
layout (location = 4) uniform float uKernelRadius;
layout (location = 5) uniform vec3 samples[64];

// parameters (you'd probably want to use them as uniforms to more easily tweak the effect)

// tile noise texture over screen based on screen dimensions divided by noise size
const vec2 noiseScale = vec2(1280.0f/4.0f, 720.0f/4.0f);


void main()
{
    // Get input for SSAO algorithm
    vec3 fragPos = texture(gPositionDepth, fragment_texcoord).xyz;
    fragPos = vec3(viewMatrix * vec4(fragPos, 1.0));

    vec3 normal = texture(gNormal, fragment_texcoord).rgb;
    vec3 randomVec = texture(texNoise, fragment_texcoord * noiseScale).xyz;
    // Create TBN change-of-basis matrix: from tangent-space to view-space
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    // Iterate over the sample kernel and calculate occlusion factor
    float occlusion = 0.0;
    for(int i = 0; i < uKernelSize; ++i)
    {
        // get kernelSampler position
        vec3 kernelSampler = TBN * samples[i]; // From tangent to view-space
        kernelSampler = fragPos + kernelSampler * uKernelRadius;

        // project kernelSampler position (to kernelSampler texture) (to get position on screen/texture)
        vec4 offset = vec4(kernelSampler, 1.0);
        offset = projectionMatrix * offset; // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0

        // get kernelSampler depth
        float sampleDepth = -texture(gPositionDepth, offset.xy).w; // Get depth value of kernel kernelSampler

        // range check & accumulate
        float rangeCheck = smoothstep(0.0, 1.0, uKernelRadius / abs(fragPos.z - sampleDepth ));
        occlusion += (sampleDepth >= kernelSampler.z ? 1.0 : 0.0) * rangeCheck;
    }
    occlusion = 1.0 - (occlusion / uKernelSize);

    FragColor = occlusion;
}
