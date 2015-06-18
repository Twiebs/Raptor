#pragma once

#include <Core/Common.hpp>
#include <Math/MathUtils.h>

template<typename T>
class UnorderedArray {
public:

	UnorderedArray(uint32 initalCapacity = 8) {
		data = new T[initalCapacity];
		Clear();
		capacity = initalCapacity;
	}

	~UnorderedArray() {
		delete[] data;
	}

	//??? Use &&
	void Add(T element) {
		if (count == capacity)
			Grow();
		data[count++] = element;
	}


	T Get(uint32 index) {
		if (index > count) {
			return 0;
		}
		if (index >= capacity)
			Grow(((index * 7) / 4) + 1);
		return data[index];
	}

	void Clear() {
		for (uint32 i = 0; i < capacity; i++) {
			data[i] = 0;
		}
	}

	//Sets the provided index to the provided element
	//Will grow the array if it needs to and doesnt care about the order at all
	void Set(uint32 index, T element) {
		if (index >= capacity)
			Grow(((index * 7) / 4) + 1);
		count = MathUtils::Max(count, index + 1);
		data[index] = element;
	}

private:
	uint32 capacity = 0;
	uint32 count = 0;
	T* data;

	void Grow() {
		Grow((count * 7) / 4 + 1);
	}

	//TODO LEEK HERE
	void Grow(uint32 newCapacity) {
		capacity = newCapacity;
		T* oldData = data;
		data = new T[newCapacity];
		Clear();
		for (uint32 i = 0; i < count; i++)
			data[i] = oldData[i];
	}
};