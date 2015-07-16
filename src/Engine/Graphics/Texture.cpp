#include "Texture.hpp"

GLuint DEBUGLoadTexture(std::string filename) {
	SDL_Surface* image = IMG_Load(filename.c_str());
	if (image == nullptr) {
		LOG_ERROR("Could not open file: " << filename << " :: " << IMG_GetError());
		return 0;
	}

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //Disable byte-alignment restriction
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->w, image->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	SDL_FreeSurface(image);
	return textureID;
}

GLuint CreateArrayTexture2D(U32 width, U32 height, U32 layerCount, std::vector<std::string>& filenames) {
	GLuint textureID = 0;
	GLuint mipLevelCount = 1;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D_ARRAY, textureID);
	//Allocate the storage.
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, mipLevelCount, GL_RGBA8, width, height, layerCount);
	//Upload pixel data.
	//The first 0 refers to the mipmap level (level 0, since there's only 1)
	//The following 2 zeroes refers to the x and y offsets in case you only want to specify a subrectangle.
	//The final 0 refers to the layer index offset (we start from index 0 and have 2 levels).
	//Altogether you can specify a 3D box subset of the overall texture, but only one mip level at a time.

	for (auto i = 0; i < layerCount; i++) {
		auto pixmap = LoadPixmap(std::string(ASSET_DIR) + filenames[i]);
		ASSERT(pixmap->width == width);
		ASSERT(pixmap->height == height);
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixmap->data);
	}

	//Always set reasonable texture parameters
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	return textureID;
}

GLuint CreateTextureFromPixels(U32 width, U32 height, U8* pixels) {
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	return textureID;
}

Vector2 GetTexelCoords(U32 x, U32 y, U32 width, U32 height) {
	F32 u = (F32)(x) / (F32)width;
	F32 v = (F32)(y) / (F32)height;
	return Vector2(u, v);
}
