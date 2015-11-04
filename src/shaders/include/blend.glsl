#define BlendOverlayf(base, blend) 	(base < 0.5 ? (2.0 * base * blend) : (1.0 - 2.0 * (1.0 - base) * (1.0 - blend)))
#define Blend(base, blend, funcf) 		vec4(funcf(base.r, blend.r), funcf(base.g, blend.g), funcf(base.b, blend.b), funcf(base.a, blend.a))
#define BlendOverlay(base, blend) 		Blend(base, blend, BlendOverlayf)

#define internal_overlay(a, b) (a < 0.5f ? (2.0 * a * b) : (1.0 - 2.0 * (1.0 - a) * 1.0 - b))
#define internal_blend(a, b, function) vec4(function(a.r, b.r), function(a.g, b.g), function(a.b, b.b))

#define overlay(a, b) internal_blend(a, b, internal_overlay)


vec4 lerp (vec4 a, vec4 b, float s) {
	return vec4(a + (b - a) * s);
}
