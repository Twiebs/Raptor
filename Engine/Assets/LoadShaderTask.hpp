#pragma once

#include<fstream>

#include <Core\AssetManager.hpp>
#include <Core\TaskManager.hpp>

class LoadShaderTask : public ITask {
	LoadShaderTask(AssetID id, AssetRegistry* registry, std::string vertSourceFile, std::string fragSourceFile);
	~LoadShaderTask();

	void Run() final;
	void Finalize() final;

private:
	AssetID id;
	AssetRegistry* registry;

	std::string vertexShaderFilename;
	std::string vertexShaderSource;
	std::string fragmentShaderFilename;
	std::string fragmentShaderSource;
};