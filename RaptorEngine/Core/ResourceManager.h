#pragma once

#include<queue>
#include<memory>
#include<thread>
#include<iostream>
#include<unordered_map>
#include<map>

//Resources
#include<Graphics\GL\GLSLProgram.h>
#include<Graphics\GL\GLTexture.h>
#include<Graphics\Model.h>
#include<UI\Font.h>
#include<Math\MathUtils.h>

//Loaders
#include<SOIL\SOIL.h>
#include<FreeImage.h>
#include<assimp\Importer.hpp>
#include<assimp\scene.h>
#include<assimp\postprocess.h>

class GLTexture;
class Model;

class ResourceManager {
public:

	//Returns the instance of the resourceManager singleton
	static ResourceManager& Instance();

	void LoadImageAsync(std::string filename);
	void LoadShaderProgram(std::string vertSourceFile, std::string fragSourceFile);

	Font* LoadFont(const std::string& filename, uint32 fontSize);

	GLTexture* GetGLTexture(std::string filename);
	ImageData* LoadImage(std::string filename);

	std::unique_ptr<Model> LoadModel(std::string filename);

	ImageData* ResourceManager::GenerateImage(int width, int height);

private:
	ResourceManager();
	~ResourceManager();

	std::unordered_map<std::string, Font*> fonts;
	std::unordered_map<std::string, std::unique_ptr<GLSLProgram>> shaders;

};

