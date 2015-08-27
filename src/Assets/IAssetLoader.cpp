#include "IAssetLoader.hpp"

LoadAssetTask::LoadAssetTask(AssetID id, AssetRegistry* registry) : 
	id(id), registry(registry) { }

LoadAssetTask::~LoadAssetTask() {

}

