#include "Texture.h"

#include<FreeImage.h>


Texture::Texture(GLenum textureTarget) {
	this->textureTarget = textureTarget;
	glGenTextures(1, &textureID);
}

Texture::~Texture() {
}

void Texture::Bind() {
	glBindTexture(textureTarget, textureID);
}

CubemapTexture::CubemapTexture(std::vector<const char*> faces) : Texture(GL_TEXTURE_CUBE_MAP) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	for (int i = 0; i < faces.size(); i++) {
		Image imageFI = ResourceManager::LoadImage(faces[i]);
		Image image = ResourceManager::LoadSOILImage(faces[i]);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, imageFI.width, imageFI.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageFI.pixels);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
