#pragma once

#include<SOIL\SOIL.h>

namespace ImageLoader {

	struct ImageData {
		int width;
		int height;
		unsigned char* bits;
	};

	void LoadSOILImage(const char* filename, ImageData* data);
}