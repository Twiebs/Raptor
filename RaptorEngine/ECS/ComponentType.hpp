#pragma once

#include<bitset>
#include<Core\Types.h>

struct ComponentType {
	std::bitset<64> bit;
	uint32 index;
	byte size;
};