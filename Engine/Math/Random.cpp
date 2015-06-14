#include "Random.h"

Random::Random() {
	Seed(0);
}
Random::Random(int seed) {
	Seed(seed);
}
Random::~Random() {
}
void Random::Seed(int seed) {
	rng.seed(0);
}

float Random::Range(float min, float max) {
	std::uniform_real_distribution<float> dist(min, max);
	return dist(rng);
}

int Random::Range(int min, int max){
	std::uniform_int_distribution<int> dist(min, max);
	return dist(rng);
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
