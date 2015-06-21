#pragma once
#include<fstream>

#include <Core/AssetManager.hpp>
#include <Core/TaskManager.hpp>

typedef uint64 AssetID;
struct AssetRegistry;

class LoadShaderTask : public ITask {
public:
	LoadShaderTask(AssetID id, AssetRegistry* registry, std::string vertSourceFile, std::string fragSourceFile);
	~LoadShaderTask();

	void Run(uint32 threadID) final;
	void Finalize(uint32 threadID) final;

private:
	AssetID id;
	AssetRegistry* registry;

	std::string vertexShaderFilename;
	std::string vertexShaderSource;
	std::string fragmentShaderFilename;
	std::string fragmentShaderSource;
};
