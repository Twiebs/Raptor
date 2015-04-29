#include "Texture.h"

#include<FreeImage.h>


Texture::Texture(GLenum textureTarget) {
	this->textureTarget = textureTarget;
	glGenTextures(1, &textureID);
}

Texture::~Texture() {
}

TextureData* Texture::LoadTexture(const char* filename) {
	FREE_IMAGE_FORMAT format = FreeImage_GetFileType(filename, 0);
	FIBITMAP* image = FreeImage_Load(format, filename);

	FIBITMAP* temp = image;
	image = FreeImage_ConvertTo32Bits(image);
	FreeImage_Unload(temp);

	int width = FreeImage_GetWidth(image);
	int height = FreeImage_GetHeight(image);

	GLubyte* texture = new GLubyte[4 * width*height];
	char* pixels = (char*)FreeImage_GetBits(image);

	for (int i = 0; i < (width * height); i++) {
		texture[i * 4 + 0] = pixels[i * 4 + 2];
		texture[i * 4 + 1] = pixels[i * 4 + 1];
		texture[i * 4 + 2] = pixels[i * 4 + 0];
		texture[i * 4 + 3] = pixels[i * 4 + 3];
	}
	
	TextureData data;
	data.filename = filename;
	data.width = FreeImage_GetWidth(image);
	data.height = FreeImage_GetHeight(image);
	data.pixels = texture;
	return &data;
}

void Texture::Bind() {
	glBindTexture(textureTarget, textureID);
}

CubemapTexture::CubemapTexture(std::vector<const char*> faces) : Texture(GL_TEXTURE_CUBE_MAP) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	for (int i = 0; i < faces.size(); i++) {
		ImageLoader::ImageData data;
		ImageLoader::LoadSOILImage(faces[i], &data);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, data.width, data.height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.bits);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
