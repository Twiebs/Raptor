in VertexOut {
	vec3 position;
	vec3 normal;
	vec2 texCoord;
	mat3 tbn;
} fs_in;

#include <include/lighting.glsl>
#include <include/deferred.glsl>

void main() {
	vec3 waterDiffuseColor = vec3(0.7, 0.8, 1.0);

	SetPosition(fs_in.position);
	SetNormal(fs_in.normal);
	SetColor(waterDiffuseColor);
	SetSpecular(32);
}
