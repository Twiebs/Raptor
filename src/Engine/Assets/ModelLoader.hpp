#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <Assets/IAssetLoader.hpp>
#include <Core/TaskManager.hpp>
#include <Core/AssetManager.hpp>
#include <ECS/Component.hpp>
#include <Graphics/Model.hpp>

struct AssetRegistry;
class LoadAssetTask;

class LoadModelTask : public ITask {
public:
	LoadModelTask(AssetID id, AssetRegistry* registry, std::string& filename);
	~LoadModelTask();

	void Run(uint32 threadID) override;
	void Finalize(uint32 threadID) override;

private:
	AssetID id;
	AssetRegistry* registry;
	std::string filename;
	Model* result;
};

class ModelLoader {
public:
	ModelLoader();
	~ModelLoader();


};
