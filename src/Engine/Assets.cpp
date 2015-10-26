#include "Assets.hpp"

#include <Core/Tasks.hpp>
#include <Core/logging.h>

global_variable AssetManifest __g_manifest;
const AssetManifest& GetGlobalAssetManifest() { 
	assert_called_by_main_thread();
	return __g_manifest;
}

static AssetManifest& GetGlobalAssetManifestInternal() {
	assert_called_by_main_thread();
	return __g_manifest;
}

const Material& GetMaterial (const MaterialHandle& handle) {
	auto& manifest = GetGlobalAssetManifestInternal();
	return manifest.materials[handle.arrayIndex];
};

MaterialHandle LoadMaterial (const std::string& filename) {
	auto& manifest = GetGlobalAssetManifestInternal();
	MaterialData data;
	LoadMaterialData(data, filename);

	manifest.materials.push_back(Material());
	manifest.materialAssetInfos.push_back(AssetInfo());
	auto materialIndex = manifest.materials.size() - 1;
	auto& material = manifest.materials.back();
	auto& info = manifest.materialAssetInfos.back();

	auto lastSlash = filename.find_last_of('/');
	if (lastSlash = std::string::npos) {
		info.name = filename;
		info.directory = "";
	} else {
		info.name = filename.substr(lastSlash, filename.length() - lastSlash);
		info.directory = filename.substr(0, lastSlash);
	}

	auto LoadMaterialTexture = [&info](MaterialParameter param, const std::string& filename, GLenum mode) {
		if (filename == "") LOG_VERBOSE("Material " << info.name << " did not contain parameter " << GetMaterialParameterString(param));
		return LoadTextureReleaseData(filename, mode);
	};

	material.diffuseMapID	= LoadMaterialTexture(MATERIAL_DIFFUSE,	data.diffuseTextureFilename, GL_REPEAT);
	material.normalMapID	= LoadMaterialTexture(MATERIAL_SPECULAR, data.normalTextureFilename, GL_REPEAT);
	material.specularMapID	= LoadMaterialTexture(MATERIAL_NORMAL, data.specularTextureFilename, GL_REPEAT);
	return MaterialHandle { materialIndex };
}
//
//MaterialHandle LoadMaterial (const std::string& diffuseFilename, const std::string& specularFilename, const std::string& normalFilename) {
//	auto& manifest = GetGlobalAssetManifestInternal();
//	manifest.materials.push_back(Material());
//	auto& material = manifest.materials.back();
//	auto materialIndex = manifest.materials.size() - 1;
//
//	material.diffuseMapID	= LoadMaterialTexture(MATERIAL_DIFFUSE, diffuseFilename, GL_REPEAT);
//	material.specularMapID	= LoadMaterialTexture(MATERIAL_SPECULAR, specularFilename, GL_REPEAT);
//	material.normalMapID	= LoadMaterialTexture(MATERIAL_NORMAL, normalFilename, GL_REPEAT);
//	return MaterialHandle{ materialIndex };
//}

ShaderHandle LoadShader (ShaderBuilder& builder) {
	auto& manifest = GetGlobalAssetManifestInternal();
	manifest.shaderBuilderData.push_back(builder.data);
	manifest.shaderPrograms.emplace_back(Shader{ builder.build() });
	auto handleArrayIndex = manifest.shaderPrograms.size() - 1;
	return ShaderHandle{ handleArrayIndex };
}

const Shader& GetShader(const ShaderHandle& handle) {
	auto& manifest = GetGlobalAssetManifest();
	return manifest.shaderPrograms[handle.arrayIndex];
}

void ReloadShader(const ShaderHandle& handle) {
	auto& manifest = GetGlobalAssetManifestInternal();
	auto& shader = manifest.shaderPrograms[handle.arrayIndex];
	auto& builderData = manifest.shaderBuilderData[handle.arrayIndex];
	glDeleteProgram(shader.id);
	shader.id = CompileShader(&builderData);
}

const Model& GetModel(const ModelHandle& handle) {
	auto& manifest = GetGlobalAssetManifestInternal();
	return *manifest.models[handle.arrayIndex];
}

ModelHandle LoadModel(const std::string& filename) {
	auto& manifest = GetGlobalAssetManifestInternal();
	auto model = ImportModel(filename);
	manifest.models.push_back(model);
	auto modelIndex = manifest.models.size() - 1;
	return ModelHandle{ modelIndex };
}