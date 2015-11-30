#include "Assets.hpp"

#include <Engine/Engine.hpp>
#include <Core/Tasks.hpp>
#include <Core/logging.h>

#include <fstream>

void AssetManifest::AddEntry(const std::string& name, const std::string& filename) {
	AssetManifestEntry entry;
	entry.name = name;
	entry.filename = filename;
	entries.emplace_back(entry);
	auto entryIndex = entries.size(); // Intentionaly 1 > then actual index
	nameToEntryIndexMap[name] = entryIndex;

	AssetState assetState;
	assetState.loadedState = AssetLoadState::UNLOADED;
	assetState.assetIndex = 0;
	assetStates.emplace_back(assetState);
}

#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>

template <class TArchive>
void serialize(TArchive& archive, AssetManifestEntry& entry) {
	archive(cereal::make_nvp("assetName", entry.name));
	archive(cereal::make_nvp("filename", entry.filename));
}

template <class TArchive>
void serialize(TArchive& archive, AssetManifest& registry) {
	archive(registry.entries);
}

void AssetManifest::Serialize() { 
	std::ofstream stream("asset_manifest.json");
	cereal::JSONOutputArchive archive(stream);
	archive(*this);
}

void AssetManifest::Deserialize() {
	std::ifstream stream("asset_manifest.json");
	cereal::JSONInputArchive archive(stream);
	archive(*this);

	for (U32 i = 0; i < entries.size(); i++) {
		AssetState state;
		state.assetIndex = 0;
		state.loadedState = AssetLoadState::UNLOADED;
		assetStates.emplace_back(state);
		
		auto& entry = entries[i];
		nameToEntryIndexMap[entry.name] = i + 1;
	}


}

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
	LoadMaterialTask(MaterialInfo& info, MaterialHandle handle);
	void execute(U32 workerID) override;
	void finalize(U32 workerID) override;
private:
	MaterialInfo info;
	MaterialData data;
	MaterialHandle handle;
};

LoadMaterialTask::LoadMaterialTask(MaterialInfo& info, MaterialHandle handle)
	: info(info), handle(handle) { }

void LoadMaterialTask::execute(U32 workerID) {
	ImportMaterialData(&data, info);
}

void LoadMaterialTask::finalize(U32 workerID) {
	auto& material = GetMaterial(handle);
	InitMaterial((Material*)&material, &data);
	FreeMaterialData(&data);
}

MaterialHandle AssetManager::LoadMaterial (const MaterialInfo& info) {
	assert(info.name != "" && "Material data must have a unique name!");
	// assert(info.directory != "" && "Material data must have a directory to load a file from!");

	materials.push_back(Material());
	materialAssetInfos.push_back(info);
	auto materialIndex = materials.size() - 1;

	auto handle = MaterialHandle { materialIndex };
	ScheduleTask<LoadMaterialTask>(info, handle);
	return handle;
}

MaterialHandle AssetManager::LoadMaterial (const std::string& filename) {
	MaterialInfo info;
	LoadMaterialInfo(&info, filename);
	return LoadMaterial(info);
}

ShaderHandle AssetManager::LoadShader (ShaderBuilder& builder) {
	shaderBuilderData.push_back(builder.data);
	shaderPrograms.emplace_back(builder.build_program());
	auto handleArrayIndex = shaderPrograms.size() - 1;
	return ShaderHandle{ handleArrayIndex };
}


void AssetManager::ReloadShader(const ShaderHandle& handle) {
	auto& shader = shaderPrograms[handle.arrayIndex];
	auto& builderData = shaderBuilderData[handle.arrayIndex];
	glDeleteProgram(shader.id);
	shader.id = CompileShader(&builderData);
	if (shader.id != 0) LOG_INFO("Sucuessfuly reloaded Shader:" << builderData.name);
}

struct LoadModelTask : public ITask {
	LoadModelTask(const std::string& filename, ModelHandle handel, AssetManager* manager);
	void execute(U32 workerID) override;
	void finalize(U32 workerID) override;
	std::string filename;
	ModelHandle handel;
	ModelData data;
	AssetManager* manager;
};

LoadModelTask::LoadModelTask(const std::string& filename, ModelHandle handel, AssetManager* manager)
	: filename(filename), handel(handel), manager(manager) { }

void LoadModelTask::execute(U32 workerID) {
	ImportModelData(&data, filename);
}

void LoadModelTask::finalize(U32 workerID) {
	manager->models[handel.arrayIndex] = CreateModel(&data);
	auto& model = manager->models[handel.arrayIndex];
	
	for (U32 i = 0; i < data.materialInfos.size(); i++) {
		// model.meshMaterialHandles[i] = MaterialHandle { 0 };
		model.meshMaterialHandles[i] = manager->LoadMaterial(data.materialInfos[i]);
	}
}

ModelHandle AssetManager::LoadModel (const std::string& assetName) {
	assert_called_by_main_thread();

	auto entryIndex = manifest->nameToEntryIndexMap[assetName];
	if (entryIndex == 0) {
		assert(false && "You are trying to look up an asset name that has not been entered into the"
			"Asset manifest that is curerntly loaded!");
	} else {
		auto& assetState = manifest->assetStates[entryIndex - 1];
		if (assetState.loadedState == AssetLoadState::LOADED) {
			auto result = ModelHandle { assetState.assetIndex };
			return result;
		} else {
			models.emplace_back();
			modelInfo.emplace_back();

			//We probably dont need this info struct anymore
			//unless we also want to import metrics like the loaded size
			// whatever else... that will probably be inside of the packfile anyway
			// so that we can do very intelligent allocations
			auto& info = modelInfo.back();


			auto& entry = manifest->entries[entryIndex - 1];
			auto filename = ASSET_DIRECTORY + entry.filename;

			//info.name = filename;
			//info.directory = filename;
			auto modelIndex = models.size() - 1;
			assetState.assetIndex = modelIndex;
			assetState.loadedState = AssetLoadState::LOADED;
			manifest->loadedCount++;
			auto handle = ModelHandle { modelIndex };
			ScheduleTask<LoadModelTask>(filename, handle, this);
			return handle;
		}
	}
}

//ModelHandle AssetManager::LoadModel (const std::string& filename) {
//	models.emplace_back();
//	modelInfo.emplace_back();
//	auto& info = modelInfo.back();
//	info.name = filename;
//	info.directory = filename;
//	auto modelIndex = models.size() - 1;
//	auto handle = ModelHandle { modelIndex };
//	ScheduleTask<LoadModelTask>(filename, handle, this);
//	return handle;
//}

