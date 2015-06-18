#pragma once

#include <Core\Common.hpp>
#include <Core\TaskManager.hpp>
#include <Core\AssetManager.hpp>

struct AssetRegistry;
class ITask;

class LoadAssetTask : public ITask {
public:
	LoadAssetTask(AssetID id, AssetRegistry* registry);
	~LoadAssetTask();

	virtual void Run() = 0;
	virtual void Finalize() = 0;

protected:
	AssetID id;
	AssetRegistry* registry;
};


class IAssetLoader {

};