//Copyright 2015 Torin Wiebelt

#pragma once
#include<glm\gtc\noise.hpp>

//Base class all noise implementations derrive from
//This class provideds maniupulation functions to preforme on all types of noise
//Derived classes must provide 2D and 3D noise; however, 1D and 4D are not required... mabye... for now...
class Noise {
public:
	virtual float Eval(float x) = 0;
	virtual float Eval(float x, float y) = 0;
	virtual float Eval(float x, float y, float z) = 0;
	virtual float Eval(float x, float y, float z, float w) = 0;

	//Fractal Brownian Motion
	float FBM(float x, int ocvtanves, float frequency, float persistance);
	float FBM(float x, float y, int octaves, float frequency, float persistance);
	float FBM(float x, float y, float z, int octaves, float frequency, float persistance);
	float FBM(float x, float y, float z, float w, int octaves, float frequency, float persistance);

	float RidgedNoise(float x, int ocvtanves, float frequency, float persistance);
	float RidgedNoise(float x, float y, int octaves, float frequency, float persistance);
	float RidgedNoise(float x, float y, float z, int octaves, float frequency, float persistance);
	float RidgedNoise(float x, float y, float z, float w, int octaves, float frequency, float persistance);
};

//Improved Perlin Noise
//Based on KenPerlins original Java implementation http://mrl.nyu.edu/~perlin/noise/
//Also from tutorial here http://flafla2.github.io/2014/08/09/perlinnoise.html
class ImprovedPerlinNoise : public Noise {
public:
	ImprovedPerlinNoise(unsigned int seed);
	~ImprovedPerlinNoise();

	float Eval(float x) override;
	float Eval(float x, float y) override;
	float Eval(float x, float y, float z) override;
	float Eval(float x, float y, float z, float w) override;
};

//Simplex noise implementation
class SimplexNoise : public Noise {
public:
	SimplexNoise(unsigned int seed);
	~SimplexNoise();

	float Eval(float x) override;
	float Eval(float x, float y) override;
	float Eval(float x, float y, float z) override;
	float Eval(float x, float y, float z, float w) override;
};

//Open Simplex noise implementation originaly created by Kurt Spencer
//His Java Implementation : https://gist.github.com/KdotJPG/b1270127455a94ac5d19
//This is based on Stephen M. Cameron port of Kurt Spencer's java implementation https://github.com/smcameron/open-simplex-noise-in-c/blob/master/open-simplex-noise.c
class OpenSimplexNoise : public Noise {
public:
	OpenSimplexNoise(unsigned int seed);
	~OpenSimplexNoise();

	float Eval(float x) override;
	float Eval(float x, float y) override;
	float Eval(float x, float y, float z) override;
	float Eval(float x, float y, float z, float w) override;
};

//Wrapper class for glm simplex noise implementation
class GLMSimplexNoise : public Noise {
public:
	GLMSimplexNoise(unsigned int seed = 0);
	~GLMSimplexNoise();

	float Eval(float x) override;
	float Eval(float x, float y) override;
	float Eval(float x, float y, float z) override;
	float Eval(float x, float y, float z, float w) override;
};