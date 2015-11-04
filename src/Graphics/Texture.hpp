#pragma once

#include <vector>
#include <string>

#include <GL/glew.h>
#include <Core/types.h>
#include <Math/Vector2.hpp>

enum class ImageFormat {
	INVALID = 0,
    RGBA8 = 4,
};

struct ImageData {
	int width, height;
	ImageFormat format;
    U8* pixels;

    ImageData(const std::string& filename);
    ~ImageData();
};


struct TextureData {
	U32 width = 0, height = 0;
	ImageFormat format = ImageFormat::INVALID;
	U8* pixels = nullptr;
};

struct Texture {
	GLuint gl_textureID;
};

struct TextureRegion {
	GLuint textureID;
	Vector2 texCoords[4];
};

struct AtlasRegion {
#if 0
	U16 x, y;
	U16 w, h;
#endif
#if 1
	F32 x, y;
	F32 right, top;
#endif
};

struct TextureAtlas {
	U16 width, height;
	U32 regionCount;
	U8* memblock;
	AtlasRegion* regions;
	U8* pixels;
};

void WriteTextureAtlasToFile(TextureAtlas* atlas, std::string filename);
void LoadTextureAtlasFromFile(TextureAtlas* atlas, std::string filename);
void FreeTextureAtlas(TextureAtlas* atlas);

GLuint CreateArrayTexture2D(U32 width, U32 height, std::vector<std::string>& filenames);
GLuint CreateTextureFromPixels(U32 width, U32 height, U8* pixels, GLenum wrapMode = GL_CLAMP_TO_EDGE);
GLuint CreateTextureFromFile(const std::string& filename);
GLuint CreateAlphaMap(U32 width, U32 height, U8* data);

GLuint LoadTextureReleaseData (const std::string& filename, GLenum wrap_mode = GL_CLAMP_TO_EDGE);
GLuint CreateRGBATexture (U32 width, U32 height, U8* data, GLenum wrap_mode = GL_CLAMP_TO_EDGE);

TextureData LoadTextureData (const std::string& filename);

Vector2 GetTexelCoords(U32 x, U32 y, U32 width, U32 height);
U8* LoadPixelsFromFile(const std::string& filename);
void WritePixelsHack(U8* dest, U16 destX, U16 destY, U16 destWidth, U16 destHeight, U8* src, U16 srcWidth, U16 srcHeight);
void WritePixels(U16 srcWidth, U16 srcHeight, U8* src, U16 destX, U16 destY, U16 destWidth, U16 destHeight, U8* dest);
//void WritePixels(U16 srcX, U16 srcY, U16 srcW, U16 srcH, U16 destX, U16 destY, U16 destW, U16 destH, U8* src, U8* dest);
