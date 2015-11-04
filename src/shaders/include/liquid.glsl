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

vec3 GetWaveNormal (float x, float z) {
  float dx = 0.0;
  float dz = 0.0;
  for (int i = 0; i < LIQUID_PARAMETER_COMPONENT_COUNT; i++) {
    dx += dWave_dx(i, x, z);
    dz += dWave_dz(i, x, z);
  }

  vec3 n = vec3(-dx, -1.0, -dz);
  return normalize(n);
}

float GetWaveHeight (float x, float z) {
	float height = 0.0;
	for (int i = 0; i < LIQUID_PARAMETER_COMPONENT_COUNT; i++) {
		height += evaluate_wave_height(i, x, z);
	}
	return height;
}
