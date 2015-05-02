#include "ResourceManager.h"


ResourceManager::ResourceManager() {
}


ResourceManager::~ResourceManager() {
}

Image ResourceManager::LoadSOILImage(std::string filename) {
	Image image;
	image.pixels = SOIL_load_image(filename.c_str(), &image.width, &image.height, 0, SOIL_LOAD_RGB);
	return image;
}

Image ResourceManager::LoadImage(std::string filename) {
	FREE_IMAGE_FORMAT format = FreeImage_GetFileType(filename.c_str(), 0);
	if (format == FIF_UNKNOWN) {
		std::cout << "ERROR: Could not read file: " << filename.c_str() << std::endl;
	}

	//Load the image a report errors if it fails
	FIBITMAP* image = FreeImage_Load(format, filename.c_str());
	if (!image) {
		std::cout << "ERROR: Image file was found and reconized but it could not be loaded";
	}

	//Convert the bitmap into useable form
	FIBITMAP* temp = FreeImage_ConvertTo32Bits(image);
	FreeImage_Unload(image);
	image = temp;

	int width = FreeImage_GetWidth(image);
	int height = FreeImage_GetHeight(image);

	char* pixels = (char*)FreeImage_GetBits(image);
	unsigned char* texture = new unsigned char[4 * width*height];

	FreeImage_FlipVertical(image);
	FreeImage_ConvertToRGBA16(image);

	for (int i = 0; i < (width * height); i++) {
		texture[i * 4 + 0] = pixels[i * 4 + 2];
		texture[i * 4 + 1] = pixels[i * 4 + 1];
		texture[i * 4 + 2] = pixels[i * 4 + 0];
		texture[i * 4 + 3] = pixels[i * 4 + 3];
	}

	Image data;
	data.filename = filename;
	data.width = FreeImage_GetWidth(image);
	data.height = FreeImage_GetHeight(image);
	data.pixels = (unsigned char*)texture;
	return data;
}
