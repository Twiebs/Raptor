#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <stdint.h>

template<typename TAsset>
struct AssetHandle {
	uint32_t arrayIndex;
	uint64_t uuid;
};

template<typename TAsset>
inline const TAsset& GetAsset(AssetHandle<TAsset> handle) {
	static_assert(false, "GetAsset was called with a generic AssetType.  All asset types must have a "
		"Template specialization function defined for this procedure");
}

template<typename TAsset>
inline AssetHandle<TAsset> ObtainAssetHandle(const std::string& asset_name) {
	static_assert(false, "ObtainAsset was called with a generic AssetType.  All asset types must have a "
		"Template specialization function for each of the procedures required by the asset manager");
}

template<typename T>
inline const char* TypeName() {
	static_assert(false, "TypeName was called with an unregistered typename!");
}


enum class AssetLoadState {
	UNLOADED,
	LOADED
};

struct AssetState {
	AssetLoadState loadedState = AssetLoadState::UNLOADED;
	uint32_t assetIndex = 0;
};

struct AssetManifestEntry {
	std::string name;
	std::string filename;
};

struct AssetManifest {
	std::vector<AssetManifestEntry> entries;

	void AddEntry(const std::string& name, const std::string& filename);

	inline int64_t GetEntryIndex(const std::string& entryName) {
		auto result = static_cast<int64_t>(nameToEntryIndexMap[entryName]) - 1;
		return result;
	}

	template<typename TAsset>
	void SerializeManifest();
	template<typename TAsset>
	void DeSerializeManifest();

private:
	std::unordered_map<std::string, uint32_t> nameToEntryIndexMap;
};

#include <cereal/archives/json.hpp>
#include <cereal/types/vector.hpp>

template <class TArchive>
void serialize(TArchive& archive, AssetManifestEntry& entry) {
	archive(cereal::make_nvp("assetName", entry.name));
	archive(cereal::make_nvp("filename", entry.filename));
}

template<typename TArchive>
void serialize(TArchive& archive, AssetManifest& manifest) {
	archive(manifest.entries);
}

template<typename TAsset>
void AssetManifest::SerializeManifest() {
	std::ofstream stream(std::string(TypeName<TAsset>()) + ".manifest");
	cereal::JSONOutputArchive archive(stream);
	archive(*this);
}

template<typename TAsset>
void AssetManifest::DeSerializeManifest() {
	std::ifstream stream(std::string(TypeName<TAsset>()) + ".manifest");
	if (!stream.is_open()) return;
	cereal::JSONInputArchive archive(stream);
	archive(*this);
}

template <typename TAsset>
struct AssetManager {
	AssetManifest manifest;
	std::vector<TAsset> assets;
	std::vector<AssetState> assetStates;

	AssetManager();
	~AssetManager();

	AssetHandle<TAsset> ObtainAssetHandle(const std::string& assetName);
	const TAsset& GetAsset(AssetHandle<TAsset> handle);
};

template<typename TAsset>
AssetManager<TAsset>::AssetManager() {
	manifest.DeSerializeManifest<TAsset>();
	assetStates.resize(manifest.entries.size());
}

template<typename TAsset>
AssetManager<TAsset>::~AssetManager() {
	manifest.SerializeManifest<TAsset>();
}

template <typename TAsset>
AssetHandle<TAsset> AssetManager<TAsset>::ObtainAssetHandle(const std::string& assetName) {
	auto entryIndex = manifest.GetEntryIndex(assetName);
	if (entryIndex >= 0) {
		auto& assetState = assetStates[entryIndex];
		if (assetState.loadedState == AssetLoadState::LOADED) {
			auto result = AssetHandle<TAsset> { assetState.assetIndex };
			return result;
		} else {
			auto& manifestEntry = manifest.entries[entryIndex];
			auto assetFilename = ASSET_DIRECTORY + manifestEntry.filename;
			
			assets.emplace_back();
			auto loadedAssetIndex = assets.size() - 1;
			assetState.loadedState = AssetLoadState::LOADED;
			assetState.assetIndex = loadedAssetIndex;
			auto handle = AssetHandle<TAsset> { loadedAssetIndex };
			__LoadAsset<TAsset>(assetFilename, handle);
			return handle;
		}
	}
}

#define DEFINE_ASSET_TYPE_NAME(TAsset) template<> inline const char* TypeName<TAsset>() { return #TAsset; }
#define TYPEDEF_ASSET_HANDLE(TAsset) typedef AssetHandle<TAsset> ##TAsset##Handle;
#define ASSET_MANAGER_NAME(TAsset) __##TAsset##Manager
#define IMPLEMENT_ASSET_MANAGER(TAsset) static AssetManager<TAsset> ASSET_MANAGER_NAME(TAsset)
#define REGISTER_ASSET(TAsset) DEFINE_ASSET_TYPE_NAME(TAsset) TYPEDEF_ASSET_HANDLE(TAsset) IMPLEMENT_ASSET_MANAGER(TAsset)

#define IMPLEMENT_ASSET_LIBARY
#ifdef IMPLEMENT_ASSET_LIBARY
//#define ASSET_MANAGER_NAME(AssetTypeName) __##AssetTypeName##_assset_manager
//#define ASSET_HANDLE_NAME(AssetTypeName) AssetTypeName##Handle
//#define ASSET_MANAGER_DECLERATION(TAssetType) static AssetManager<TAssetType> ASSET_MANAGER_NAME(TAssetType)
//#define ASSET_HANDLE_DECLERATION(TAssetType) typedef AssetHandle<TAssetType> ASSET_HANDLE_NAME(TAssetType)
//#define GET_ASSET_DEFINITION(TAssetType) template<> const TAssetType& GetAsset<TAssetType>(AssetHandle<TAssetType> handle) { return ASSET_MANAGER_NAME(TAssetType).assets[handle.arrayIndex]; }
//#define IMPLEMENT_ASSET(TAssetType) ASSET_MANAGER_DECLERATION(TAssetType); \
//ASSET_HANDLE_DECLERATION(TAssetType); \
//GET_ASSET_DEFINITION(TAssetType)

void AssetManifest::AddEntry(const std::string& name, const std::string& filename) {
	AssetManifestEntry entry;
	entry.name = name;
	entry.filename = filename;
	entries.emplace_back(entry);
	auto entryIndex = entries.size(); // Intentionaly 1 > then actual index
	nameToEntryIndexMap[name] = entryIndex;
}

#endif