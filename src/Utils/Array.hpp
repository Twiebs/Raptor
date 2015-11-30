#ifndef RAPTOR_ARRAY_HPP
#define RAPTOR_ARRAY_HPP
#include <Core/Common.hpp>


template<typename T>
class StaticArray {
public:
	StaticArray(size_t count);
	~StaticArray();

	T& operator [](const size_t index) {
		assert(index < count && "StaticArray subscript out of bounds!");
		return elements[index];
	}

	size_t Count() {
		return count;
	}
private:
	T* elements;
	size_t count;
};



template<typename T>
class Array {
public:
	Array();
	Array(size_t initialCapacity);
	Array(size_t initialCapacity, const T& defaultValue);
	~Array();

	T& operator [](const size_t index) {
		assert(index <= count && "Array subscript out of bounds!");
		return data[index];
	}

	void Add(const T& element);

	template<typename... Args>
	void Create(Args... args) {
		if (count + 1 > capacity)
			Grow((capacity * 2) + 10);
		new (&data[count]) T(args...);
		count += 1;
	}

	template<typename... Args>
	T& CreateAndReturn(Args... args) {
		if (count + 1 > capacity)
			Grow((capacity * 2) + 10);
		T& element = data[count];
		new (&element) T(args...);
		count += 1;
		return element;
	}

	void Resize(size_t size);

	void Remove();

	T& PopAndReturn() {
		auto& element = data[count - 1];
		count--;
		return element;
	}

	size_t Count() {
		return count;
	}

private:
	T* data;
	size_t capacity;
	size_t count;

	void Grow(size_t newCapacity);
};


template<typename T>
Array<T>::Array() {
	data = nullptr;
	count = 0;
	capacity = 0;
}

template<typename T>
Array<T>::Array(size_t initialCapacity) {
	data = (T*)malloc(sizeof(T) * initialCapacity);
	count = 0;
	capacity = initialCapacity;
}

template<typename T>
Array<T>::Array(size_t initialCapacity, const T& defaultValue) {
	data = (T*)malloc(sizeof(T) * initialCapacity);
	count = initialCapacity;
	capacity = initialCapacity;
	for (U32 i = 0; i < count; i++) {
		data[i] = defaultValue;
	}
}

template<typename T>
Array<T>::~Array() {
	free(data);
}

template<typename T>
void Array<T>::Add (const T& element) {
	assert(data != nullptr);
	if (count + 1 > capacity) Grow(capacity * 2);
	data[count - 1] = element;
	count++;
}

template<typename T>
void Array<T>::Grow(size_t newCapacity) {
	void* newData = malloc(sizeof(T) * newCapacity);
	memcpy(newData, data, sizeof(T) * count);
	if (data != nullptr) free(data);
	capacity = newCapacity;
	data = (T*)newData;
}

template<typename T>
void Array<T>::Resize(size_t newSize) {
	assert(newSize > count && "Resize cannot shrink an array that has a larger count");
	void* newData = malloc(sizeof(T) * newSize);
	if (data != nullptr) {
		memcpy(newData, data, sizeof(T) * count);
		free(data);
		data = (T*)newData;
		capacity = newSize;
		count = newSize;
	} else {
		data = (T*)newData;
		capacity = newSize;
		count = newSize;
	}
}
#endif //RAPTOR_ARRAY_HPP
