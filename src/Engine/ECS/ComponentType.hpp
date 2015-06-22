#pragma once

#include <bitset>
#include <Core/Common.hpp>

struct ComponentType {
	std::bitset<64> flags;
	std::bitset<64> bit;
	uint32 index;
	uint32 size;	//This could be enormous uint8 is unessecary
};
