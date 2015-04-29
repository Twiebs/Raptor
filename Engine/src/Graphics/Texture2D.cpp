#include <iostream>
#include "Texture2D.h"
#include<FreeImage.h>


Texture2D::Texture2D(char* filename) {
	this->textureTarget = GL_TEXTURE_2D;
	this->filename = filename;
}

bool Texture2D::Load() {
	FREE_IMAGE_FORMAT format = FreeImage_GetFileType(filename, 0);
	FIBITMAP* image = FreeImage_Load(format, filename);

	FIBITMAP* temp = image;
	image = FreeImage_ConvertTo32Bits(image);
	FreeImage_Unload(temp);

	int width = FreeImage_GetWidth(image);
	int height = FreeImage_GetHeight(image);
	std::cout << "Loaded: " << filename << " | " << width << "*" << height << std::endl;

	GLubyte* texture = new GLubyte[4*width*height];
	char* pixels = (char*)FreeImage_GetBits(image);

	for (int i = 0; i < (width * height); i++) {
		texture[i * 4 + 0] = pixels[i * 4 + 2];
		texture[i * 4 + 1] = pixels[i * 4 + 1];
		texture[i * 4 + 2] = pixels[i * 4 + 0];
		texture[i * 4 + 3] = pixels[i * 4 + 3];
	}

	glGenTextures(1, &textureObject);
	glBindTexture(textureTarget, textureObject);
	glTexImage2D(textureTarget, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*) texture);
	glTexParameterf(textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(textureTarget, 0);

	return true;
}

void Texture2D::Bind(GLenum textureUnit) {
	glActiveTexture(textureUnit);
	glBindTexture(textureTarget, textureObject);
}