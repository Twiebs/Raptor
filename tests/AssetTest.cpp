#include <iostream>
#include <assets/assets.hpp>


#include <Graphics/Model.hpp>

#include <Core/Tasks.hpp>

REGISTER_ASSET(Material);
REGISTER_ASSET(Model);

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

struct LoadModelTask : public ITask {
	LoadModelTask(const std::string& filename, U32 arrayIndex);
	void execute(U32 workerID) override;
	void finalize(U32 workerID) override;
	std::string filename;
	U32 arrayIndex;
	ModelData data;
};

LoadModelTask::LoadModelTask(const std::string& filename, U32 arrayIndex)
	: filename(filename), arrayIndex(arrayIndex) { }

void LoadModelTask::execute(U32 workerID) {
	ImportModelData(&data, filename);
}

void LoadModelTask::finalize(U32 workerID) {
	auto& manager = ASSET_MANAGER_NAME(Model);
	manager.assets[arrayIndex] = CreateModel(&data);
	auto& model = manager.assets[arrayIndex];
	for (U32 i = 0; i < data.materialInfos.size(); i++) {
		// model.meshMaterialHandles[i] = MaterialHandle { 0 };
		// model.meshMaterialHandles[i] = manager->LoadMaterial(data.materialInfos[i]);
	}
}

inline void LoadModel(const std::string& filename, U32 arrayIndex) {



}



int main() {
	return 0;
}