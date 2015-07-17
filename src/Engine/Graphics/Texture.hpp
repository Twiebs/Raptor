#pragma once

#include <vector>
#include <string>

#include <GL/glew.h>
#include <Core/Common.hpp>
#include <Math/Vector2.hpp>

struct Pixmap {
	U16 width, height;
	U8* pixels;
};

struct TextureRegion {
	GLuint textureID;
	Vector2 texCoords[4];
};

struct AtlasRegion {
	U16 x, y;
	U16 w, h;
};

struct TextureAtlas {
	U16 width, height;
	U32 regionCount;
	U8* memblock;
	AtlasRegion* regions;
	U8* pixels;
};

void LoadPixmapFromFile(Pixmap* pixmap, std::string filename);

void WriteTextureAtlasToFile(TextureAtlas* atlas, std::string filename);
void ReadTextureAtlasFromFile(TextureAtlas* atlas, std::string filename);

GLuint CreateArrayTexture2D(U32 width, U32 height, U32 layerCount, std::vector<std::string>& filenames);
GLuint CreateTextureFromPixels(U32 width, U32 height, U8* pixels);
GLuint CreateTextureFromFile(std::string filename);

Vector2 GetTexelCoords(U32 x, U32 y, U32 width, U32 height);
void WritePixelsHack(U8* dest, U16 destX, U16 destY, U16 destWidth, U16 destHeight, U8* src, U16 srcWidth, U16 srcHeight);
void WritePixels(U16 srcWidth, U16 srcHeight, U8* src, U16 destX, U16 destY, U16 destWidth, U16 destHeight, U8* dest);
//void WritePixels(U16 srcX, U16 srcY, U16 srcW, U16 srcH, U16 destX, U16 destY, U16 destW, U16 destH, U8* src, U8* dest);