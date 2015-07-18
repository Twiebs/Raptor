#include "Texture.hpp"

#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

void LoadPixmapFromFile(Pixmap* pixmap, std::string filename) {
	pixmap->pixels = stbi_load(filename.c_str(), (int*)&pixmap->width, (int*)&pixmap->height, nullptr, 4);
}

U8* GetPixelAddress(U8* pixels, U16 x, U16 y, U16 width, U16 height) {
	return &pixels[((y* width) + x) * 4];
}

void WritePixelsHack(U8* dest, U16 destX, U16 destY, U16 destWidth, U16 destHeight, U8* src, U16 srcWidth, U16 srcHeight) {
	for (auto ix = 0; ix < srcWidth; ix++) {
		for (auto iy = 0; iy < srcHeight; iy++) {
			auto srcPixel = GetPixelAddress(src, ix, iy, srcWidth, srcHeight);
			auto destPixel = GetPixelAddress(dest, destX + ix, destY + iy, destWidth, destHeight);
			memcpy(destPixel, srcPixel, 4);
		}
	}
}

void WritePixels(U16 srcWidth, U16 srcHeight, U8* src, U16 destX, U16 destY, U16 destWidth, U16 destHeight, U8* dest) {
	for (U16 y = 0; y < srcHeight; y++) {
		U8* srcPix = GetPixelAddress(src, 0, y, srcWidth, srcHeight);
		U8* destPix = GetPixelAddress(dest, destX, destY + y, destWidth, destHeight);
		memcpy(destPix, srcPix, srcWidth * 4);
	}
}

#if 1
void LoadTextureAtlasFromFile(TextureAtlas* atlas, std::string filename) {
	std::ifstream stream;
	stream.open(filename);
	if (!stream.is_open()) { LOG_ERROR("HUSTON WE HAVE A PROBLEM"); return; }
	stream.read((char*)&atlas->width, sizeof(U16) * 2);
	stream.read((char*)&atlas->regionCount, sizeof(atlas->regionCount));
	auto regionMemSize = sizeof(AtlasRegion) * atlas->regionCount;
	auto pixelMemSize = atlas->width * atlas->height * 4;
	atlas->memblock = new U8[regionMemSize + pixelMemSize];
	atlas->regions = (AtlasRegion*)atlas->memblock;
	atlas->pixels = (U8*)(atlas->memblock + regionMemSize);
	stream.read((char*)atlas->regions, regionMemSize);
	auto lastDot = filename.find_last_of(".");
	auto base = filename.substr(0, filename.length() - (filename.length() - lastDot));
	atlas->pixels = stbi_load((base + ".png").c_str(), (int*)&atlas->width, (int*)&atlas->height, nullptr, 4);
}

void WriteTextureAtlasToFile(TextureAtlas* atlas, std::string filename) {
	std::ofstream stream;
	stream.open(filename, std::ios_base::binary);
	if (!stream.is_open()) { LOG_ERROR("HUSTON WE HAVE A PROBLEM"); return; }
	stream.write((const char*)&atlas->width, sizeof(U16) * 2);
	stream.write((const char*)&atlas->regionCount, sizeof(atlas->regionCount));
	stream.write((const char*)atlas->regions, sizeof(AtlasRegion) * atlas->regionCount);
	stream.close();
	auto lastDot = filename.find_last_of(".");
	auto base = filename.substr(0, filename.length() - (filename.length() - lastDot));
	stbi_write_png((base + ".png").c_str(), atlas->width, atlas->height, 4, atlas->pixels, atlas->width * 4);
}

void FreeTextureAtlas(TextureAtlas* atlas) {
	stbi_image_free(atlas->pixels);
}

#endif

#if 0	//Binary encoding of raw pixel data
void LoadTextureAtlasFromFile(TextureAtlas* atlas, std::string filename) {
	std::ifstream stream;
	stream.open(filename);
	if (!stream.is_open()) { LOG_ERROR("HUSTON WE HAVE A PROBLEM"); return; }
	stream.read((char*)&atlas->width, sizeof(U16) * 2);
	stream.read((char*)&atlas->regionCount, sizeof(atlas->regionCount));
	auto regionMemSize = sizeof(AtlasRegion) * atlas->regionCount;
	auto pixelMemSize = atlas->width * atlas->height * 4;
	atlas->memblock = new U8[regionMemSize + pixelMemSize];
	atlas->regions = (AtlasRegion*)atlas->memblock;
	atlas->pixels = (U8*)(atlas->memblock + regionMemSize);
	stream.read((char*)atlas->regions, regionMemSize);
	stream.read((char*)atlas->pixels, pixelMemSize);
}

void WriteTextureAtlasToFile(TextureAtlas* atlas, std::string filename) {
	std::ofstream stream;
	stream.open(filename, std::ios_base::binary);
	if (!stream.is_open()) { LOG_ERROR("HUSTON WE HAVE A PROBLEM"); return; }
	stream.write((const char*)&atlas->width, sizeof(U16) * 2);
	stream.write((const char*)&atlas->regionCount, sizeof(atlas->regionCount));
	stream.write((const char*)atlas->regions, sizeof(AtlasRegion) * atlas->regionCount);
	stream.write((const char*)atlas->pixels, atlas->width * atlas->height * 4);
	stream.close();
}
#endif

GLuint CreateTextureFromFile(std::string filename) {
	int width, height;
	U8* pixels = stbi_load(filename.c_str(), &width, &height, nullptr, 4);

	auto textureID = CreateTextureFromPixels(width, height, pixels);
	stbi_image_free(pixels);
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
		Pixmap pixmap;
		LoadPixmapFromFile(&pixmap, std::string(ASSET_DIR) + filenames[i]);
		ASSERT(pixmap.width == width);
		ASSERT(pixmap.height == height);
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixmap.pixels);
		delete[] pixmap.pixels;
	}

	//Always set reasonable texture parameters
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	return textureID;
}

//GLuint CreateTextureFromPixmap(Pixmap* pixmap) {
//	return CreateTextureFromPixels(pixmap->width, pixmap->height, pixmap->data);
//}

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

//TODO benchmark this...
//Your being stupid.  Check out the preformace implications of hitting this in the wrong order
//void WritePixels(U16 srcX, U16 srcY, U16 srcTotalW, U16 srcTotalH, U16 destX, U16 destY, U16 destTotalW, U16 destTotalH, U8* src, U8* dest) {
//	for (auto iy = 0; iy < srcTotalH; iy++) {
//		for (auto ix = 0; ix < srcTotalW; ix++) {
//			auto srcPixel = &src[(((iy + srcY) * srcTotalW) + (ix + srcX)) * 4];
//			auto destPixel = &dest[(((iy + destY) * destTotalW) + (ix + destX)) * 4];
//			memcpy(destPixel, srcPixel, 4);
//		}
//	}
//}
