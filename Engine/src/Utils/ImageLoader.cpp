
#include"ImageLoader.h"

namespace ImageLoader {

	void LoadSOILImage(const char* filename, ImageData* data) {
		unsigned char* image = SOIL_load_image(filename, &data->width, &data->height, 0, SOIL_LOAD_RGB);
		data->bits = image;
	}

}