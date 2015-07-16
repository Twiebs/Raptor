#pragma once

#include <Core/Common.hpp>
#include <stdlib.h>

//Represents a image in memory.  Stores metrics of width, height, and the
//Format of the image
//TODO should not store pixmap info with pixmap
//32 bit descriptor for a pixmap;
#define PIXMAP_SIZE_TYPE U16


struct PixmapDescriptor {
	const char* filename;
	U16 width, height;
};

//struct Pixmap {
//	U16 width, height;
//	U8* data;
//

//};

class Pixmap {
public:
	Pixmap() { }
	Pixmap(U16 width, U16 height) {
		this->width = width;
		this->height = height;
		data = new uint8[width * height * 4];
	}

	Pixmap(U16 width, U16 height, U8* dataToCopy) {
		this->width = width;
		this->height = height;
		data = new uint8[width * height * 4];
//		memcpy(data, dataToCopy, width * height * 4);
	}

	~Pixmap() {
		delete[] data;
	}

	U8* GetPixelAddress(U16 x, U16 y) {
		return &data[((y * width) + x) * 4];
	}

	U16 width, height;
	U8* data;
};
