#pragma once

#include <Core/Common.hpp>

enum class PixmapFormat : uint8 {
	RGB8 = 3,
	RGBA8 = 4,
};

//Represents a image in memory.  Stores metrics of width, height, and the
//Format of the image
class Pixmap {
public:
	Pixmap() { }
	Pixmap(uint16 width, uint16 height, PixmapFormat format) {
		this->width = width;
		this->height = height;
		this->format = format;
		data = new uint8[width * height * (uint8)format];
	}

	~Pixmap() {
		delete[] data;
	}

	uint16 width, height;
	PixmapFormat format;
	uint8* data;
};
