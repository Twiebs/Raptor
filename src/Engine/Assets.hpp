#pragma once

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

#define ASSET_FILE(filename)  ASSET_DIRECTORY  filename
#define SHADER_FILE(filename) SHADER_DIRECTORY filename

#define LoadShaderFromBuilder() LoadShader(_builder)

#define AssetTypeHandleName(assetTypeName) assetTypeName##Handle

#define __GenerateAssetHandleStruct(handleName) struct handleName { U32 arrayIndex; }
#define __GenerateAssetLoadFunction(assetName, handleName) handleName Load##assetName (const std::string& filename);
#define __GenerateAssetReloadFunction(assetName, handleName) void Reload##assetName (const handleName & handle)
#define __GenerateAssetGetFunction(assetName, handleName) const assetName & Get##assetName(const handleName & handle);

#define __GenerateAsset(assetName) \
	__GenerateAssetHandleStruct(AssetTypeHandleName(assetName)); \
	__GenerateAssetLoadFunction(assetName, AssetTypeHandleName(assetName)); \
	__GenerateAssetGetFunction(assetName, AssetTypeHandleName(assetName))

__GenerateAsset(Texture);
__GenerateAsset(Material);
__GenerateAsset(Shader);
__GenerateAsset(Model);
__GenerateAsset(Sound);
__GenerateAsset(Music);

struct AssetInfo {
	std::string name;
	std::string directory;
};

struct AssetManifest {
	
	std::vector<ShaderBuilderData> shaderBuilderData;
	std::vector<AssetInfo> materialAssetInfos;
	
	std::vector<Shader> shaderPrograms;
	std::vector<Material> materials;
	std::vector<Texture> textures;
	std::vector<Model*> models;
};

const AssetManifest& GetGlobalAssetManifest();

const Texture& GetTexture(const TextureHandle& handle);
TextureHandle LoadTexture(const std::string& filename);

const Material& GetMaterial(const MaterialHandle& handle);
MaterialHandle LoadMaterial(const std::string& filename);
// MaterialHandle LoadMaterial(const std::string& diffuseFilename, const std::string& specularFilename, const std::string& normalFilename);

const Shader& GetShader(const ShaderHandle& handle);
ShaderHandle LoadShader(ShaderBuilder& builder);

//const Model& GetModel(const ModelHandle& handle);
//ModelHandle LoadModel(const std::string& filename);


#ifdef ENABLE_ASSET_RELOADING
void ReloadTexture  (const TextureHandle& handle);
void ReloadMaterial (const MaterialHandle& handle);
void ReloadShader   (const ShaderHandle& handle);
#endif