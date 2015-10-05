#ifndef RAPTOR_ARRAY_HPP
#define RAPTOR_ARRAY_HPP
#include <Core/Common.hpp>

template<typename T>
class Array {

};

template<typename T>
class StaticArray {
public:
	StaticArray(T* elements);
	~StaticArray();
private:
	T* elements;
	U32 count;
};

#endif //RAPTOR_ARRAY_HPP
