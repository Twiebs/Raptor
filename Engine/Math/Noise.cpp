#include "Noise.h"

//Noise
#pragma region NoiseFunctions
float Noise::FBM(float x, float y, int octaves, float frequency, float persistance) {
	float total = 0.0f;
	float amplitude = 1.0f;
	float maxAmp = 0.0f;

	for (int i = 0; i < octaves; i++) {
		total += Eval(x * frequency, y * frequency) * amplitude;
		frequency *= 2.0f;
		maxAmp += amplitude;
		amplitude *= persistance;
	}
	return total / maxAmp;
};
#pragma endregion

#pragma region ImprovedPerlinNoise
float ImprovedPerlinNoise::Eval(float x, float y){
	int xi = (int)x % 255;
	int yi = (int)y % 255;
	int xf = x - (int)x;
	int yf = y - (int)y;

	//float u = Fade(xf);
	//float v = Fade(yf);
	return 0;
}

float Fade(float t){
	return t * t * t * (t * 6 - 15) + 10;
}
#pragma endregion

#pragma region GLMSimplexNoise
GLMSimplexNoise::GLMSimplexNoise(unsigned int seed){
	//Cant set a seed :(
}
GLMSimplexNoise::~GLMSimplexNoise(){

}
float GLMSimplexNoise::Eval(float x){
	return 0;
}
float GLMSimplexNoise::Eval(float x, float y){
	return (glm::simplex(glm::vec2(x, y)) + 1) * 0.5f;
}
float GLMSimplexNoise::Eval(float x, float y, float z){
	return glm::simplex(glm::vec3(x, y, z));
}
float GLMSimplexNoise::Eval(float x, float y, float z, float w){
	return glm::simplex(glm::vec4(x, y, z, w));
}
#pragma endregion