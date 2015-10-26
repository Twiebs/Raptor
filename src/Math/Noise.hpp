//Copyright 2015 Torin Wiebelt

#pragma once

#include <Math/Math.hpp>

//Base class all noise implementations derrive from
//This class provideds maniupulation functions to preforme on all types of noise
//Derived classes must provide 2D and 3D noise; however, 1D and 4D are not required... mabye... for now...
class Noise {
public:
	virtual ~Noise() { }

	virtual float Eval(float x) const = 0;
	virtual float Eval(float x, float y) const = 0;
	virtual float Eval(float x, float y, float z) const = 0;
	virtual float Eval(float x, float y, float z, float w) const = 0;

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

	float Eval(float x) const final;
	float Eval(float x, float y) const final;
	float Eval(float x, float y, float z) const final;
	float Eval(float x, float y, float z, float w) const final;
};

//Simplex noise implementation
class SimplexNoise : public Noise {
public:
	SimplexNoise(unsigned int seed);
	virtual ~SimplexNoise();

	float Eval(float x) const final;
	float Eval(float x, float y) const final;
	float Eval(float x, float y, float z) const final;
	float Eval(float x, float y, float z, float w) const final;
};

//Open Simplex noise implementation ported from Kurt Spencers OpenSimplexNoise
// https://gist.github.com/KdotJPG/b1270127455a94ac5d19
class OpenSimplexNoise : public Noise {
public:
	OpenSimplexNoise(U64);
	virtual ~OpenSimplexNoise() { }

	float Eval(float x) const final;
	float Eval(float x, float y) const final;
	float Eval(float x, float y, float z) const final;
	float Eval(float x, float y, float z, float w) const final;

private:
	U16 perm[256];
	U16 permGradIndex3D[256];

	F64 Extrapolate(int xsb, int ysb, double dx, double dy) const;
};
