#include "ComponentBlock.hpp"

ComponentBlock::ComponentBlock(ComponentType* type) {
	dataSize = type->size;
}

ComponentBlock::~ComponentBlock() {

}


void ComponentBlock::Grow() {
	Grow((capacity * 2) + 10);
}

void ComponentBlock::Grow(uint32 newCapacity) {
	capacity = newCapacity;

	byte* newData = new byte[dataSize * capacity];
	std::memcpy(newData, data, count*dataSize);

	delete[] data;
	data = newData;
}

void* ComponentBlock::Get(uint32 index) const {
	return (byte*)data + index * dataSize;
}

uint32 ComponentBlock::Count() const {
	return count;
}