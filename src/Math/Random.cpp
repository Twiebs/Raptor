#include "Random.hpp"

Random::Random() {
	Seed(0);
}
Random::Random(int seed) {
	Seed(seed);
}
Random::~Random() {
}
void Random::Seed(int seed) {
	rng.seed(seed);
}

float Random::Range(float min, float max) {
	std::uniform_real_distribution<float> dist(min, max);
	return dist(rng);
}

int Random::Range(int min, int max){
	std::uniform_int_distribution<S32> dist(min, max);
	return dist(rng);
}

Vector2 Random::PointInRectangle(const Rectangle& rectangle) {
	std::uniform_real_distribution<float> distX(rectangle.x, rectangle.x + rectangle.w);
	std::uniform_real_distribution<float> distY(rectangle.y, rectangle.y + rectangle.h);
	return Vector2(distX(rng), distY(rng));
}

V3 Random::PointInCuboid (const Cuboid& cube) {
	std::uniform_real_distribution<float> distX(cube.x, cube.x + cube.w);
	std::uniform_real_distribution<float> distY(cube.y, cube.y + cube.h);
	std::uniform_real_distribution<float> distZ(cube.z, cube.z + cube.l);
	V3 result = V3(distX(rng), distY(rng), distZ(rng));
	return result;
}

V3 Random::vector3() {
	std::uniform_real_distribution<float> dist(0.0f, 1.0f);
	V3 result = V3(dist(rng), dist(rng), dist(rng));
	return result;
}


//void Random::Seed(int seed) {
//	index = 0;
//	mt[0] = seed;
//	for (int i = 1; i < 623; i++) {
//		mt[i] = 1812433253 * (mt[i - 1] ^ (mt[i - 1] << 30) + 1);
//	}
//}

//int Random::nextInt() {
//	if (index == 0) {
//		GenerateNumbers();
//	}
//
//	int y = mt[index];
//	y = y ^ (y >> 11);
//	y = y ^ (y << 7) & (2636928640);
//	y = y ^ (y << 15) & (4022730752);
//	y = y ^ (y >> 18);
//
//	index = (index + 1) % 624;
//	return y;
//}
//
//void Random::GenerateNumbers() {
//	for (int i = 0; i < 623; i++) {
//		int y = (mt[i] & 0x80000000) + mt[(i + 1) % 624] & 0x7fffffff;
//		mt[i] = mt[(i + 397) % 624] ^ (y >> 1);
//		if (y % 2 != 0) {
//			mt[i] = mt[i] ^ (2567483615);
//		}
//	}
//}
