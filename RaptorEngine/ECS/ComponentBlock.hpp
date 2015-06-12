#pragma once

#include<new>
#include<Core\Types.h>
#include<ECS\ComponentType.hpp>

class ComponentBlock {
public:
	ComponentBlock(ComponentType* type);
	~ComponentBlock();

	template<typename T>
	void* Alloc() {
		if (count >= capacity)
			Grow();

		void* component = (byte*)data + count++ * dataSize;
		new (component) T();
		return component;
	}
	void* Free(void* memory);

	void* Get(uint32)	const;
	uint32 Capacity()	const;
	uint32 Count()		const;

private:
	uint32 capacity;
	uint32 count = 0;
	uint32 dataSize;
	byte* data;

	void Grow();
	void Grow(uint32 newCapacity);
};

