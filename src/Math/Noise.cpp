#include "Noise.hpp"

static const F64 STRETCH_CONSTANT_2D = -0.211324865405187;    //(1/Math.sqrt(2+1)-1)/2;
static const F64 SQUISH_CONSTANT_2D = 0.366025403784439;      //(Math.sqrt(2+1)-1)/2;
static const F64 STRETCH_CONSTANT_3D = -1.0 / 6;              //(1/Math.sqrt(3+1)-1)/3;
static const F64 SQUISH_CONSTANT_3D = 1.0 / 3;                //(Math.sqrt(3+1)-1)/3;
static const F64 STRETCH_CONSTANT_4D = -0.138196601125011;    //(1/Math.sqrt(4+1)-1)/4;
static const F64 SQUISH_CONSTANT_4D = 0.309016994374947;      //(Math.sqrt(4+1)-1)/4;

static const F64 NORM_CONSTANT_2D = 47;
static const F64 NORM_CONSTANT_3D = 103;
static const F64 NORM_CONSTANT_4D = 30;

static const F64 DEFAULT_SEED = 0;

//Gradients for 2D. They approximate the directions to the
//vertices of an octagon from the center.
static const S8 gradients2D[16]{
	5, 2, 2, 5,
	-5, 2, -2, 5,
	5, -2, 2, -5,
	-5, -2, -2, -5,
};

//Noise
float Noise::FBM (float x, float y, int octaves, float frequency, float persistance) {
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

float Noise::RidgedNoise(float x, float y, int octaves, float frequency, float persistance) {
	float total = 0.0f;
	float amplitude = 1.0f;
	float maxAmp = 0.0f;
	for (int i = 0; i < octaves; i++) {
		total += ((1.0f - std::abs(Eval(x*frequency, y*frequency))) * 2.0f - 1.0f) * amplitude;
		frequency *= 2.0f;
		maxAmp += amplitude;
		amplitude *= persistance;
	}
	return total / maxAmp;
}



float ImprovedPerlinNoise::Eval(float x, float y) const {
	return 0;
}

float Fade(float t){
	return t * t * t * (t * 6 - 15) + 10;
}


//OpenSimplex
OpenSimplexNoise::OpenSimplexNoise(U64 seed) {
	U16 source[256];
	for (U16 i = 0; i < 256; i++)
		source[i] = i;

	seed = seed * 6364136223846793005 + 1442695040888963407;
	seed = seed * 6364136223846793005 + 1442695040888963407;
	seed = seed * 6364136223846793005 + 1442695040888963407;

	for (int i = 255, j = 0; i >= 0; i--, j++) {
		seed = seed * 6364136223846793005l + 1442695040888963407l;
		int r = (int)((seed + 31) % (i + 1));
		if (r < 0)
			r += (i + 1);
		perm[i] = source[r];
		permGradIndex3D[i] = (short)((perm[i] % (255 / 3)) * 3);
		source[r] = source[i];
	}
}

F64 OpenSimplexNoise::Extrapolate(int xsb, int ysb, double dx, double dy) const {
	int index = perm[(perm[xsb & 0xFF] + ysb) & 0xFF] & 0x0E;
	return gradients2D[index] * dx
		+ gradients2D[index + 1] * dy;

}

float OpenSimplexNoise::Eval(float x) const {
	return 0;
}

float OpenSimplexNoise::Eval (float x, float y) const {

	//Place input coordinates onto grid.
	double stretchOffset = (x + y) * STRETCH_CONSTANT_2D;
	double xs = x + stretchOffset;
	double ys = y + stretchOffset;

	//Floor to get grid coordinates of rhombus (stretched square) super-cell origin.
	int xsb = FastFloor(xs);
	int ysb = FastFloor(ys);

	//Skew out to get actual coordinates of rhombus origin. We'll need these later.
	double squishOffset = (xsb + ysb) * SQUISH_CONSTANT_2D;
	double xb = xsb + squishOffset;
	double yb = ysb + squishOffset;

	//Compute grid coordinates relative to rhombus origin.
	double xins = xs - xsb;
	double yins = ys - ysb;

	//Sum those together to get a value that determines which region we're in.
	double inSum = xins + yins;

	//Positions relative to origin point.
	double dx0 = x - xb;
	double dy0 = y - yb;

	//We'll be defining these inside the next block and using them afterwards.
	double dx_ext, dy_ext;
	int xsv_ext, ysv_ext;

	double value = 0;

	//Contribution (1,0)
	double dx1 = dx0 - 1 - SQUISH_CONSTANT_2D;
	double dy1 = dy0 - 0 - SQUISH_CONSTANT_2D;
	double attn1 = 2 - dx1 * dx1 - dy1 * dy1;
	if (attn1 > 0) {
		attn1 *= attn1;
		value += attn1 * attn1 * Extrapolate(xsb + 1, ysb + 0, dx1, dy1);
	}

	//Contribution (0,1)
	double dx2 = dx0 - 0 - SQUISH_CONSTANT_2D;
	double dy2 = dy0 - 1 - SQUISH_CONSTANT_2D;
	double attn2 = 2 - dx2 * dx2 - dy2 * dy2;
	if (attn2 > 0) {
		attn2 *= attn2;
		value += attn2 * attn2 * Extrapolate(xsb + 0, ysb + 1, dx2, dy2);
	}

	if (inSum <= 1) { //We're inside the triangle (2-Simplex) at (0,0)
		double zins = 1 - inSum;
		if (zins > xins || zins > yins) { //(0,0) is one of the closest two triangular vertices
			if (xins > yins) {
				xsv_ext = xsb + 1;
				ysv_ext = ysb - 1;
				dx_ext = dx0 - 1;
				dy_ext = dy0 + 1;
			} else {
				xsv_ext = xsb - 1;
				ysv_ext = ysb + 1;
				dx_ext = dx0 + 1;
				dy_ext = dy0 - 1;
			}
		} else { //(1,0) and (0,1) are the closest two vertices.
			xsv_ext = xsb + 1;
			ysv_ext = ysb + 1;
			dx_ext = dx0 - 1 - 2 * SQUISH_CONSTANT_2D;
			dy_ext = dy0 - 1 - 2 * SQUISH_CONSTANT_2D;
		}
	} else { //We're inside the triangle (2-Simplex) at (1,1)
		double zins = 2 - inSum;
		if (zins < xins || zins < yins) { //(0,0) is one of the closest two triangular vertices
			if (xins > yins) {
				xsv_ext = xsb + 2;
				ysv_ext = ysb + 0;
				dx_ext = dx0 - 2 - 2 * SQUISH_CONSTANT_2D;
				dy_ext = dy0 + 0 - 2 * SQUISH_CONSTANT_2D;
			} else {
				xsv_ext = xsb + 0;
				ysv_ext = ysb + 2;
				dx_ext = dx0 + 0 - 2 * SQUISH_CONSTANT_2D;
				dy_ext = dy0 - 2 - 2 * SQUISH_CONSTANT_2D;
			}
		} else { //(1,0) and (0,1) are the closest two vertices.
			dx_ext = dx0;
			dy_ext = dy0;
			xsv_ext = xsb;
			ysv_ext = ysb;
		}
		xsb += 1;
		ysb += 1;
		dx0 = dx0 - 1 - 2 * SQUISH_CONSTANT_2D;
		dy0 = dy0 - 1 - 2 * SQUISH_CONSTANT_2D;
	}

	//Contribution (0,0) or (1,1)
	double attn0 = 2 - dx0 * dx0 - dy0 * dy0;
	if (attn0 > 0) {
		attn0 *= attn0;
		value += attn0 * attn0 * Extrapolate(xsb, ysb, dx0, dy0);
	}

	//Extra Vertex
	double attn_ext = 2 - dx_ext * dx_ext - dy_ext * dy_ext;
	if (attn_ext > 0) {
		attn_ext *= attn_ext;
		value += attn_ext * attn_ext * Extrapolate(xsv_ext, ysv_ext, dx_ext, dy_ext);
	}

	return (float)(value / NORM_CONSTANT_2D);

}
float OpenSimplexNoise::Eval(float x, float y, float z) const {
	return 0;
}
float OpenSimplexNoise::Eval(float x, float y, float z, float w) const {
	return 0;
}



