#version 330
#include <noise3D.glsl>
in vec2 vertPosition;
out vec4 fragColor;

uniform float cloudTime;

bool IsPointInCircle(float x, float y, float centerX, float centerY, float radius) {
	bool result = ((x-centerX)*(x-centerX))
				  +((y-centerY)*(y-centerY))
				  < (radius*radius);
	return result;
}

void main () {
  if(IsPointInCircle(vertPosition.x, vertPosition.y, 512, 512, 512)) {
    float noise = OctaveNoise(vec3(vertPosition.xy, 3*cloudTime), 5, 0.001, 0.5f);
    fragColor = vec4(1.0, 1.0, 1.0, 1.0 * noise);
  } else {
    fragColor = vec4(0.0);
  }

}
