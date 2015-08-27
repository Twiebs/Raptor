#pragma once

#include <cstring>
#include <new>
#include <assert.h>

#include <Core/Common.hpp>
#include <ECS/ComponentType.hpp>

class ComponentBlock {
public:
	~ComponentBlock();

	template<typename T, typename ... Args>
	void* Alloc(Args ... args) {
		assert(dataSize > 0);
		if (count >= capacity)
			Grow();

		void* component = (uint8*) data + count++ * dataSize;
		new (component) T(args...);
		return component;
	}

	void SetDataSize(uint32 size);
	void* Free(void* memory);

	void* Get(uint32) const;
	uint32 Capacity() const;
	uint32 Count() const;

private:
	uint32 capacity = 0;
	uint32 count = 0;
	uint32 dataSize = 0;
	uint8* data = nullptr;

	void Grow();
	void Grow(uint32 newCapacity);
};
