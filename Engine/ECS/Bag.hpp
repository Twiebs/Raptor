#pragma once

#include<Core\Types.h>
#include<assert.h>

template<typename T>
class Bag {
public:
	Bag() : Bag(64) { }
	Bag(uint32 capacity) : capacity(capacity) {
		data = new T[capacity];
	}

	~Bag() {

	}

	//Adds a new element to the bag
	T& Add(T t) {
		//Grow the backing array if the current count is equal to the capacity
		if (count == capacity) Grow();	
		data[count++] = T;
		return data[count - 1];
	}

	T* Set(uint32 index, T t) {
		if (index >= capacity) Grow(index * 2);
		count++;
		data[index] = t;
		return &data[index];
	}

	T* Get(int index) {
		assert(index < capacity);
		return &data[index];
	}

	T* SafeGet(int index) {
		if (index >= capacity){
			Grow((index * 7) / 4 + 1);
		}
		return &data[index];
	}

	uint32 GetCapacity() { return capacity; }
	uint32 GetCount() { return count; }

private:
	uint32 capacity;
	uint32 count;
	T* data;


	void Grow() {
		uint32 newCapacity = (capacity * 3.0f) * 0.5f + 1.0f;
		Grow(newCapacity);
	}

	//Grows the backing array
	void Grow(int newCapacity) {
		T* newData = new T[newCapacity];
		for (uint32 i = 0; i < count; i++)
			newData[i] = data[i];
		//TODO set nullptr?

		delete[] data; //delete the old data
		capacity = newCapacity;
		data = newData;	//Move the data ptr to the new data
	}
};

