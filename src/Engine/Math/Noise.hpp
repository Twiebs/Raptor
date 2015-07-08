//Copyright 2015 Torin Wiebelt

#pragma once
#include <Math/MathUtils.hpp>

//Base class all noise implementations derrive from
//This class provideds maniupulation functions to preforme on all types of noise
//Derived classes must provide 2D and 3D noise; however, 1D and 4D are not required... mabye... for now...
class Noise {
public:
	virtual ~Noise() { }

	virtual float Eval(float x) = 0;
	virtual float Eval(float x, float y) = 0;
	virtual float Eval(float x, float y, float z) = 0;
	virtual float Eval(float x, float y, float z, float w) = 0;

	//Fractal Brownian Motion
	float FBM(float x, int octaves, float frequency, float persistance);
	float FBM(float x, float y, int octaves, float frequency, float persistance);
	float FBM(float x, float y, float z, int octaves, float frequency, float persistance);
	float FBM(float x, float y, float z, float w, int octaves, float frequency, float persistance);

	//TODO There are multiple different params that actualy go into the formation of RidgedNoise
	//Find out what they are and change the values that are in the .cpp
	//RidgedNoise
	float RidgedNoise(float x, int octaves, float frequency, float persistance);
	float RidgedNoise(float x, float y, int octaves, float frequency, float persistance);
	float RidgedNoise(float x, float y, float z, int octaves, float frequency, float persistance);
	float RidgedNoise(float x, float y, float z, float w, int octaves, float frequency, float persistance);
};

//Improved Perlin Noise
//Based on KenPerlins original Java implementation http://mrl.nyu.edu/~perlin/noise/
//Also from tutorial here http://flafla2.github.io/2014/08/09/perlinnoise.hpptml
class ImprovedPerlinNoise : public Noise {
public:
	ImprovedPerlinNoise(unsigned int seed);
	virtual ~ImprovedPerlinNoise();

	float Eval(float x) final;
	float Eval(float x, float y) final;
	float Eval(float x, float y, float z) final;
	float Eval(float x, float y, float z, float w) final;
};

//Simplex noise implementation
class SimplexNoise : public Noise {
public:
	SimplexNoise(unsigned int seed);
	virtual ~SimplexNoise();

	float Eval(float x) final;
	float Eval(float x, float y) final;
	float Eval(float x, float y, float z) final;
	float Eval(float x, float y, float z, float w) final;
};

//Open Simplex noise implementation ported from Kurt Spencers OpenSimplexNoise
// https://gist.github.com/KdotJPG/b1270127455a94ac5d19
class OpenSimplexNoise : public Noise {
public:
	OpenSimplexNoise(uint64);
	virtual ~OpenSimplexNoise() { }

	float Eval(float x) final;
	float Eval(float x, float y) final;
	float Eval(float x, float y, float z) final;
	float Eval(float x, float y, float z, float w) final;

private:
	uint16 perm[256];
	uint16 permGradIndex3D[256];

	float64 Extrapolate(int xsb, int ysb, double dx, double dy);

	const float64 STRETCH_CONSTANT_2D = -0.211324865405187;    //(1/Math.sqrt(2+1)-1)/2;
	const float64 SQUISH_CONSTANT_2D = 0.366025403784439;      //(Math.sqrt(2+1)-1)/2;
	const float64 STRETCH_CONSTANT_3D = -1.0 / 6;              //(1/Math.sqrt(3+1)-1)/3;
	const float64 SQUISH_CONSTANT_3D = 1.0 / 3;                //(Math.sqrt(3+1)-1)/3;
	const float64 STRETCH_CONSTANT_4D = -0.138196601125011;    //(1/Math.sqrt(4+1)-1)/4;
	const float64 SQUISH_CONSTANT_4D = 0.309016994374947;      //(Math.sqrt(4+1)-1)/4;

	const float64 NORM_CONSTANT_2D = 47;
	const float64 NORM_CONSTANT_3D = 103;
	const float64 NORM_CONSTANT_4D = 30;

	const uint64 DEFAULT_SEED = 0;

	//Gradients for 2D. They approximate the directions to the
	//vertices of an octagon from the center.
	const int8 gradients2D[16] = {
		 5,  2,    2,  5,
		-5,  2,   -2,  5,
		 5, -2,    2, -5,
		-5, -2,   -2, -5,
	};

};
