layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 tangent;

layout (location = 0) uniform mat4 modelMatrix;
layout (location = 1) uniform mat4 viewMatrix;
layout (location = 2) uniform mat4 projectionMatrix;

out VertexOut {
	vec3 position;
	vec3 normal;
	vec2 texCoord;
	mat3 tbn;
} vs_out;

#define PI 3.14159265359

#define LIQUID_PARAMETER_LOCATION 10
#define LIQUID_PARAMETER_COMPONENT_COUNT 8

layout (location = LIQUID_PARAMETER_LOCATION) uniform float wave_time;
layout (location = (LIQUID_PARAMETER_LOCATION + 1) + (LIQUID_PARAMETER_COMPONENT_COUNT * 0)) uniform float wave_amplitudes[LIQUID_PARAMETER_COMPONENT_COUNT];
layout (location = (LIQUID_PARAMETER_LOCATION + 1) + (LIQUID_PARAMETER_COMPONENT_COUNT * 1)) uniform float wave_lengths[LIQUID_PARAMETER_COMPONENT_COUNT];
layout (location = (LIQUID_PARAMETER_LOCATION + 1) + (LIQUID_PARAMETER_COMPONENT_COUNT * 2)) uniform float wave_speeds[LIQUID_PARAMETER_COMPONENT_COUNT];
layout (location = (LIQUID_PARAMETER_LOCATION + 1) + (LIQUID_PARAMETER_COMPONENT_COUNT * 3)) uniform vec2 wave_directions[LIQUID_PARAMETER_COMPONENT_COUNT];

float dWave_dx (int i, float x, float z) {
  float frequency = (2 * PI) / wave_lengths[i];
  float phase = wave_speeds[i] * frequency;
  float theta = dot (wave_directions[i], vec2(x, z));
  float a = wave_amplitudes[i] * wave_directions[i].x * frequency;
  return a * cos((theta * frequency) + (wave_time * phase));
}

float dWave_dz (int i, float x, float z) {
  float frequency = (2 * PI) / wave_lengths[i];
  float phase = wave_speeds[i] * frequency;
  float theta = dot (wave_directions[i], vec2(x, z));
  float a = wave_amplitudes[i] * wave_directions[i].y * frequency;
  return a * cos((theta * frequency) + (wave_time * phase));
}

float evaluate_wave_height (int i, float x, float y) {
	float frequency = (2 * PI) / wave_lengths[i];
	float phase = wave_speeds[i] * frequency;
	float theta = dot(wave_directions[i], vec2(x, y));
	return wave_amplitudes[i] * sin((theta * frequency) + (wave_time * phase));
}

vec3 GetSurfaceNormal (float x, float z) {
  float dx = 0.0;
  float dz = 0.0;
  for (int i = 0; i < LIQUID_PARAMETER_COMPONENT_COUNT; i++) {
    dx += dWave_dx(i, x, z);
    dz += dWave_dz(i, x, z);
  }

  vec3 n = vec3(-dx, -1.0, -dz);
  return normalize(n);
}

float GetSurfaceHeight(float x, float z) {
	float height = 0.0;
	for (int i = 0; i < LIQUID_PARAMETER_COMPONENT_COUNT; i++) {
		height += evaluate_wave_height(i, x, z);
	}
	return height;
}

void main() {
  vec3 bitangent = cross(tangent, normal);
  vec3 t = normalize(vec3(modelMatrix * vec4(tangent, 0.0)));
  vec3 b = normalize(vec3(modelMatrix * vec4(bitangent, 0.0)));
  vec3 n = normalize(vec3(modelMatrix * vec4(normal, 0.0)));
  vs_out.tbn = mat3(t, b, n);

	vec3 finalPosition = position;
	finalPosition.y += GetSurfaceHeight(finalPosition.x, finalPosition.z);

  vs_out.texCoord = texCoord;
  vs_out.position = vec3(modelMatrix * vec4(finalPosition, 1.0));
  vs_out.normal = GetSurfaceNormal(finalPosition.x, finalPosition.z);
  // vs_out.normal = normal;
  gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(finalPosition, 1.0f);
}
