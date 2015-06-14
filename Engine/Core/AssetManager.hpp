#pragma once

#include <Core\Types.h>
#include <memory>
#include <unordered_map>
#include <string>
#include <atomic>
#include <mutex>

//Assets
#include<Graphics\Image.h>
#include<UI\Font.h>

//Loaders
#include<FreeImage.h>
#include<typeinfo>
#include <Math\MathUtils.h>
#include <Core\TaskManager.hpp>

#include <Assets\LoadShaderTask.hpp>

typedef uint64 AssetID;
struct AssetRegistry {
	std::vector<void*> data;
};

class LoadAssetTask : public ITask {
public:
	LoadAssetTask(AssetID id, AssetRegistry* registry, std::string filename);
private:
	AssetID id;
	AssetRegistry* registry;
	std::string filename;
};

class LoadFontTask : public ITask {
public:
	LoadFontTask(AssetID id, AssetRegistry* registry, std::string filename, uint32 fontsize);
	~LoadFontTask();

	void Run() override;
	void Finalize() override;

private:
	Font* font;
	uint8* bitmapFont;
	AssetID id;
	AssetRegistry* registry;
	std::string filename;
	uint32 fontsize;
};


class AssetManager {
public:
	static AssetManager& Instance();


	//@Deprecaed
	std::shared_ptr<ImageData> GetImage(const std::string& filename);

	template<typename T>
	T* GetAsset(AssetID id) {
		T* assetPtr = (T*)registry.data[id];
		return assetPtr;
	}

	AssetID LoadFont(const std::string& filename, uint32 fontSize);
	AssetID LoadShader(const std::string& vertexShaderFile, const std::string& fragmentShaderFile);

private:


	AssetRegistry registry;

	AssetManager();
	~AssetManager();
};

