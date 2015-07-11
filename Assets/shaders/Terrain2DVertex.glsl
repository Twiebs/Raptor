attribute vec2 position;
attribute vec2 uv;
attribute vec4 color;

varying vec2 fragUV;
varying vec4 fragColor;

uniform mat4 projection;
uniform float waveAngle;
uniform bool isWater;

void main() {
  float amp = 0.3;
  fragUV = uv;
  fragColor = color;
 vec2 wavePos = vec2(position.x + (amp * cos(waveAngle) * 0.5 * sin(waveAngle + position.x + position.y)), position.y + (amp * sin(waveAngle) * cos(waveAngle + position.x + position.y)));
  // vec2 wavePos = vec2(position.x + cos(waveAngle), position.y + sin(waveAngle));
  gl_Position = projection * vec4(isWater ? wavePos : position.xy, 0.0, 1.0);
}
