#include "ComponentBlock.hpp"


ComponentBlock::~ComponentBlock() {

}

void ComponentBlock::SetDataSize(uint32 size) {
	this->dataSize = size;
}

void ComponentBlock::Grow() {
	Grow((capacity * 2) + 10);
}



void ComponentBlock::Grow(uint32 newCapacity) {
	capacity = newCapacity;

	uint8* newData = new uint8[dataSize * capacity];
	std::memcpy(newData, data, count * dataSize);

	//TODO Data is not being deleted here!
	//delete[] data;
	data = newData;
}

void* ComponentBlock::Get(uint32 index) const {
	return (uint8*) data + index * dataSize;
}

uint32 ComponentBlock::Count() const {
	return count;
}
