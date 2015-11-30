#pragma once

#include <unordered_map>

#include <Core/types.h>
#include <Core/config.h>

#include <Graphics/GLSLProgram.hpp>
#include <Graphics/Texture.hpp>
#include <Graphics/Material.hpp>
#include <Graphics/Mesh.hpp>
#include <Graphics/Model.hpp>
#include <Resources/Sound.hpp>
#include <Resources/Music.hpp>

#ifndef ASSET_DIRECTORY
static_assert(false, "Must define asset directory!");
#endif

#ifndef SHADER_DIRECTORY
static_assert(false, "Must define shader directory!");
#endif

#define ASSET_FILE(filename)  ASSET_DIRECTORY "/" filename
#define SHADER_FILE(filename) SHADER_DIRECTORY filename

#define LoadShaderFromBuilder() LoadShader(_builder)

#define AssetTypeHandleName(assetTypeName) assetTypeName##Handle

#define __GenerateAssetHandleStruct(handleName) struct handleName { U32 arrayIndex; handleName(U32 arrayIndex) : arrayIndex(arrayIndex) { } handleName() { } }
#define __GenerateAssetLoadFunction(assetName, handleName) handleName Load##assetName (const std::string& filename);
#define __GenerateAssetReloadFunction(assetName, handleName) void Reload##assetName (const handleName & handle)
#define __GenerateAssetGetFunction(assetName, handleName) const assetName & Get##assetName(const handleName & handle);

#define __GenerateAsset(assetName) \
	__GenerateAssetHandleStruct(AssetTypeHandleName(assetName)); \
	__GenerateAssetLoadFunction(assetName, AssetTypeHandleName(assetName)); \
	__GenerateAssetGetFunction(assetName, AssetTypeHandleName(assetName))

__GenerateAsset(Material);
__GenerateAsset(Texture);
__GenerateAsset(Shader);
__GenerateAsset(Model);
__GenerateAsset(Sound);
__GenerateAsset(Music);

#define ASSET_NAME(name) #name

//typedef TAssetHandle<Material> MaterialHandle;
//typedef TAssetHandle<Material> TextureHandle;
//typedef TAssetHandle<Material> ShaderHandle;
//typedef TAssetHandle<Material> ModelHandle;
//typedef TAssetHandle<Material> SoundHandle;
//typedef TAssetHandle<Material> MusicHandle;

enum AssetTypes {
	ASSET_MATERIAL = 0,
	ASSET_SHADER = 1,
	ASSET_MODEL = 2,
};

enum class AssetType {
	SHADER,
	TEXTURE,
	MATERIAL,
	MODEL
};

struct AssetInfo {
	std::string name;
	std::string directory;
};

enum class AssetLoadState {
	UNLOADED,
	LOADED,
};

struct AssetManifestEntry {
	std::string name;
	std::string filename;
};

struct AssetState {
	AssetLoadState loadedState;
	U32 assetIndex;
};

struct AssetManifest {
	std::vector<AssetManifestEntry> entries;
	std::unordered_map<std::string, U32> nameToEntryIndexMap;

	std::vector<AssetState> assetStates;
	U32 loadedCount;

	void AddEntry(const std::string& name, const std::string& filename);
	void Serialize();
	void Deserialize();
};

struct EditorAssetInfo {
	std::vector<U8> isHotloadEnabled;
};


struct AssetManager {
	EditorAssetInfo editorAssetInfo;


	std::vector<ShaderBuilderData> shaderBuilderData;
	std::vector<MaterialInfo> materialAssetInfos;
	std::vector<AssetInfo> modelInfo;

	std::vector<Shader> shaderPrograms;
	std::vector<Material> materials;
	std::vector<Texture> textures;
	std::vector<Model> models;

	AssetManifest* manifest;

	AssetManager() { }
	AssetManager(AssetManifest* manifest) : manifest(manifest) { }

	// Asset Load / Get / Reload procedures

	MaterialHandle LoadMaterial(const MaterialInfo& data);
	ShaderHandle LoadShader(ShaderBuilder& builder);

	MaterialHandle LoadMaterial(const std::string& name);
	TextureHandle LoadTexture(const std::string& name);
	ModelHandle LoadModel(const std::string& name);

	void ReloadTexture(const TextureHandle& handle);
	void ReloadMaterial(const MaterialHandle& handle);
	void ReloadShader(const ShaderHandle& handle);

	inline const Material& GetMaterial(const MaterialHandle& handle);
	inline const Texture& GetTexture(const TextureHandle& handle);
	inline const Shader& GetShader(const ShaderHandle& handle);
	inline const Model& GetModel(const ModelHandle& handle);
};

inline const Shader& AssetManager::GetShader(const ShaderHandle& handle) {
	return shaderPrograms[handle.arrayIndex];
}

inline const Model& AssetManager::GetModel(const ModelHandle& handle) {
	return models[handle.arrayIndex];
}

inline const Material& AssetManager::GetMaterial(const MaterialHandle& handle) {
	auto& result = materials[handle.arrayIndex];
	return result;
}

inline const Texture& AssetManager::GetTexture(const TextureHandle& handle) {
	auto& result = textures[handle.arrayIndex];
	return result;
}