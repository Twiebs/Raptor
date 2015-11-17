#include "Assets.hpp"

#include <Engine/Engine.hpp>
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

// TODO materials should be able to be compleatly generated programaticly
// MaterialData should contain actual pixels for the textures that were loaded
// Then we should have a materialInfo struct that allows you to specify the filename / asset loading metadata
// And thats the parth that will get compiled away.  The Material data struct will be compleatly real!.


//MaterialHandle LoadMaterial (const MaterialAssetInfo& data) {
//	assert(data.name != "" && "Material data must have a unique name!");
//	assert(data.directory != "" && "Material data must have a directory to load a file from!");
//
//	auto& manifest = GetGlobalAssetManifestInternal();
//	manifest.materials.push_back(Material());
//	manifest.materialAssetInfos.push_back(AssetInfo());
//	auto materialIndex = manifest.materials.size() - 1;
//	auto& material = manifest.materials.back();
//	auto& info = manifest.materialAssetInfos.back();
//	info.name = data.name;
//	info.directory = data.directory;
//
//	auto LoadMaterialTexture = [&info](MaterialParameter param, const std::string& filename, GLenum mode) -> GLuint {
//		if (filename == "") {
//			LOG_VERBOSE("Material " << info.name << " did not contain parameter " << GetMaterialParameterString(param));
//			return 0;
//		}
//
//		return LoadTextureReleaseData(info.directory + filename, mode);
//	};
//
//	material.diffuseMapID = LoadMaterialTexture(MATERIAL_DIFFUSE, data.diffuseTextureFilename, GL_REPEAT);
//	material.normalMapID = LoadMaterialTexture(MATERIAL_SPECULAR, data.normalTextureFilename, GL_REPEAT);
//	material.specularMapID = LoadMaterialTexture(MATERIAL_NORMAL, data.specularTextureFilename, GL_REPEAT);
//	return MaterialHandle { materialIndex };
//}

struct LoadMaterialTask : public ITask {
	LoadMaterialTask(MaterialAssetInfo& info, MaterialHandle handle);
	void execute(U32 workerID) override;
	void finalize(U32 workerID) override;
private:
	MaterialAssetInfo info;
	MaterialData data;
	MaterialHandle handle;
};

LoadMaterialTask::LoadMaterialTask(MaterialAssetInfo& info, MaterialHandle handle) 
	: info(info), handle(handle) { }

void LoadMaterialTask::execute(U32 workerID) {
	ImportMaterialData(&data, info);
}

void LoadMaterialTask::finalize(U32 workerID) {
	auto& material = GetMaterial(handle);
	InitMaterial((Material*)&material, &data);
	FreeMaterialData(&data);
}

MaterialHandle LoadMaterial (const MaterialAssetInfo& info) {
	assert(info.name != "" && "Material data must have a unique name!");
	// assert(info.directory != "" && "Material data must have a directory to load a file from!");

	auto& manifest = GetGlobalAssetManifestInternal();
	manifest.materials.push_back(Material());
	manifest.materialAssetInfos.push_back(info);
	auto materialIndex = manifest.materials.size() - 1;

	auto handle = MaterialHandle { materialIndex };
	Engine::ScheduleTask<LoadMaterialTask>(info, handle);
	return handle;
}

MaterialHandle LoadMaterial (const std::string& filename) {
	MaterialAssetInfo info;
	LoadMaterialAssetInfo(&info, filename);
	return LoadMaterial(info);
}

ShaderHandle LoadShader (ShaderBuilder& builder) {
	auto& manifest = GetGlobalAssetManifestInternal();
	manifest.shaderBuilderData.push_back(builder.data);
	manifest.shaderPrograms.emplace_back(builder.build_program());
	auto handleArrayIndex = manifest.shaderPrograms.size() - 1;
	return ShaderHandle{ handleArrayIndex };
}


const Shader& GetShader (const ShaderHandle& handle) {
	auto& manifest = GetGlobalAssetManifest();
	return manifest.shaderPrograms[handle.arrayIndex];
}

void ReloadShader(const ShaderHandle& handle) {
	auto& manifest = GetGlobalAssetManifestInternal();
	auto& shader = manifest.shaderPrograms[handle.arrayIndex];
	auto& builderData = manifest.shaderBuilderData[handle.arrayIndex];
	glDeleteProgram(shader.id);
	shader.id = CompileShader(&builderData);
	if (shader.id != 0) LOG_INFO("Sucuessfuly reloaded Shader:" << builderData.name);
}

struct LoadModelTask : public ITask {
	LoadModelTask(const std::string& filename, ModelHandle handel);
	void execute(U32 workerID) override;
	void finalize(U32 workerID) override;
	std::string filename;
	ModelHandle handel;
	ModelData data;
};

LoadModelTask::LoadModelTask(const std::string& filename, ModelHandle handel)
	: filename(filename), handel(handel) { }

void LoadModelTask::execute(U32 workerID) {
	ImportModelData(&data, filename);
}

void LoadModelTask::finalize(U32 workerID) {
	auto& manifset = GetGlobalAssetManifestInternal();
	manifset.models[handel.arrayIndex] = CreateModel(&data);
	auto& model = manifset.models[handel.arrayIndex];
	
	for (U32 i = 0; i < data.materialInfos.size(); i++) {
		// model.meshMaterialHandles[i] = MaterialHandle { 0 };
		model.meshMaterialHandles[i] = LoadMaterial(data.materialInfos[i]);
	}
}

const Model& GetModel (const ModelHandle& handle) {
	auto& manifest = GetGlobalAssetManifestInternal();
	return manifest.models[handle.arrayIndex];
}

const AssetInfo& GetModelAssetInfo(ModelHandle handle) {
	auto& manifest = GetGlobalAssetManifestInternal();
	return manifest.modelInfo[handle.arrayIndex];
}

ModelHandle LoadModel (const std::string& filename) {
	auto& manifest = GetGlobalAssetManifestInternal();
	manifest.models.emplace_back();
	manifest.modelInfo.emplace_back();
	auto& info = manifest.modelInfo.back();
	info.name = filename;
	info.directory = filename;
	auto modelIndex = manifest.models.size() - 1;
	auto handle = ModelHandle { modelIndex };

	Engine::ScheduleTask<LoadModelTask>(filename, handle);

	return handle;
}


//ModelHandle LoadModel(const std::string& filename) {
//	auto& manifest = GetGlobalAssetManifestInternal();
//	auto model = ImportModel(filename);
//	manifest.models.push_back(model);
//	auto modelIndex = manifest.models.size() - 1;
//	return ModelHandle{ modelIndex };
//}