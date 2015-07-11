precision mediump float;

varying vec2 fragUV;
varying vec4 fragColor;

uniform sampler2D sampler;
uniform bool isTexture;

#define BlendOverlayf(base, blend) 	(base < 0.5 ? (2.0 * base * blend) : (1.0 - 2.0 * (1.0 - base) * (1.0 - blend)))
#define Blend(base, blend, funcf) 		vec4(funcf(base.r, blend.r), funcf(base.g, blend.g), funcf(base.b, blend.b), funcf(base.a, blend.a))
#define BlendOverlay(base, blend) 		Blend(base, blend, BlendOverlayf)

void main() {
  vec4 result;
  vec4 textureColor = texture2D(sampler, fragUV);
  result = vec4(BlendOverlay(fragColor, textureColor).rgb, fragColor.a);
  gl_FragColor = result;
}
