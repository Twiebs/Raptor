#pragma once

#include <memory>
#include <unordered_map>
#include <string>
#include <atomic>
#include <mutex>
#include <typeinfo>

#include <FreeImage.h>

#include <freetype/ft2build.h>
#include FT_FREETYPE_H

#if _MSC_VER
#include <freetype/ft2build.h>
#else
//#include <freetype2/ft2build.h>
#endif

#include <Core/IService.hpp>
#include <Core/Common.hpp>
#include <Core/TaskManager.hpp>

#include <Math/MathUtils.hpp>
#include <Graphics/Vertex3D.hpp>

#include <Assets/Font.hpp>
#include <Assets/ModelLoader.hpp>
#include <Assets/LoadShaderTask.hpp>
#include <Assets/Pixmap.hpp>

#include <ECS/ECSManager.hpp>

class IService;

//AssetSlot template with current
//TODO:
//asset tags

enum class AssetState {
	Unloaded,
	Queued,
	Loaded,
	Locked
};

struct AssetSlot {
	AssetState state;
	void* data;
};

struct AssetRegistry {
	std::vector<void*> data;
};


class LoadFontTask : public ITask {
public:
	LoadFontTask(AssetID id, AssetRegistry* registry, std::string filename, uint32 fontsize);
	~LoadFontTask();


	void Run(uint32 threadID) final;
	void Finalize(uint32 threadID) final;

private:
	Font* font;
	uint8* bitmapFont;
	AssetID id;
	AssetRegistry* registry;
	std::string filename;
	uint32 fontsize;
};

class LoadPixmapTask : public ITask {
public:
	LoadPixmapTask(AssetID, AssetRegistry* registry, std::string& filename);
	~LoadPixmapTask();

	void Run(uint32 threadID) override;
	void Finalize(uint32 threadID) override;
private:
	AssetID id;
	AssetRegistry* registry;
	std::string filename;
	Pixmap* pixmap;
};

//TODO default memory arena size?
class AssetManager : public IService {
public:
	AssetManager();
	~AssetManager();

	static AssetManager& Instance() {
		static AssetManager* instance;
		if (instance == nullptr)
			instance = new AssetManager();
		return *instance;
	}

	template<typename T>
	T* GetAsset(AssetID id) {
		assert(id != 0);
		T* assetPtr = (T*)registry.data[id - 1];
		return assetPtr;
	}

	AssetID LoadFont(const std::string& filename, uint32 fontSize);
	AssetID LoadShader(const std::string& vertexShaderFile, const std::string& fragmentShaderFile);
	AssetID LoadModel(const std::string& filename);
	AssetID LoadPixmap(const std::string& filename);
private:
	AssetRegistry registry;

	std::vector<Font> fonts;

	AssetID GetNextAvaiableID();
};


//
//glActiveTexture(GL_TEXTURE0);
//glBindTexture(GL_TEXTURE_CUBE_MAP, id);
//for (int i = 0; i < images.size(); i++) {
//	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, images[i]->width, images[i]->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, images[i]->pixels);
//}
//
//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
//glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
