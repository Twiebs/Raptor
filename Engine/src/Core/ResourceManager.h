#pragma once

#include<iostream>

#include<SOIL\SOIL.h>
#include<FreeImage.h>

struct Image {
	//TODO format
	std::string filename;
	int width;
	int height;
	unsigned char* pixels;
};

class ResourceManager {
public:
	ResourceManager();
	~ResourceManager();

	static Image LoadSOILImage(std::string filename);
	static Image LoadImage(std::string filename);
};

