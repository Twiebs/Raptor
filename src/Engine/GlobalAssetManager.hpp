#pragma once

#include <Engine/Assets.hpp>

const Material& GetMaterial(const MaterialHandle& handle);
MaterialHandle LoadMaterial(const MaterialAssetInfo& data);
MaterialHandle LoadMaterial(const std::string& filename);
MaterialHandle LoadMaterial(const std::string& diffuseFilename, const std::string& specularFilename, const std::string& normalFilename);

const Shader& GetShader(const ShaderHandle& handle);
ShaderHandle LoadShader(ShaderBuilder& builder);

const Model& GetModel(const ModelHandle& handle);
ModelHandle LoadModel(const std::string& filename);

void ReloadTexture  (const TextureHandle& handle);
void ReloadMaterial (const MaterialHandle& handle);
void ReloadShader   (const ShaderHandle& handle);

AssetManifest* GetGlobalAssetManifest();
AssetManager* GetGlobalAssetManager();

#if defined(IMPLEMENT_GLOBAL_ASSET_MANAGER)
#undef IMPLEMENT_GLOBAL_ASSET_MANAGER
global_variable AssetManifest assetManifest;
global_variable AssetManager assetManager(&assetManifest);
AssetManifest* GetGlobalAssetManifest() { return &assetManifest; }
AssetManager* GetGlobalAssetManager() { return &assetManager; }
const Model& GetModel(const ModelHandle& handle) { return assetManager.GetModel(handle); }
ModelHandle LoadModel(const std::string& filename) { return assetManager.LoadModel(filename); }
const Shader& GetShader(const ShaderHandle& handle) { return assetManager.GetShader(handle); }
ShaderHandle LoadShader(ShaderBuilder& builder) { return assetManager.LoadShader(builder); }
const Material& GetMaterial(const MaterialHandle& handle) { return assetManager.GetMaterial(handle); }
MaterialHandle LoadMaterial(const MaterialAssetInfo& data) { return assetManager.LoadMaterial(data); }
MaterialHandle LoadMaterial(const std::string& filename) { return assetManager.LoadMaterial(filename); }
void ReloadShader(const ShaderHandle& handle) { assetManager.ReloadShader(handle); }
#endif // IMPLEMENT_GLOBAL_ASSET_MANAGER