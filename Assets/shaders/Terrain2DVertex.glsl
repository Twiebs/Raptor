#version 330 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec3 texCoord;
layout (location = 2) in vec4 color;

uniform mat4 projection;
uniform float waveAngle;
uniform bool isWater;

out vec3 fragTexCoord;
out vec4 fragColor;

#define AMP 0.3
#define FREQ 1.0

void main() {
  fragTexCoord = texCoord;
  fragColor = color;
  vec2 wavePos = vec2(position.x + (AMP * cos(FREQ * waveAngle) * 0.5 * sin(FREQ * waveAngle + position.x + position.y)), position.y + (AMP * sin(FREQ * waveAngle) * cos(FREQ * waveAngle + position.x + position.y)));
  // vec2 wavePos = vec2(position.x + cos(waveAngle), position.y + sin(waveAngle));
  gl_Position = projection * vec4(isWater ? wavePos : position.xy, 0.0, 1.0);
}
