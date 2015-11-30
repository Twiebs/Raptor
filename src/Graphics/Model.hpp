#pragma once

#include <Graphics/Material.hpp>
// #include <Engine/Assets.hpp>

struct MaterialHandle;


//typedef std::string AssetFileIdentifer;
//struct MaterialTextureMapInfo {
//	AssetFileIdentifer name;
//};



struct Model {
	MaterialHandle* meshMaterialHandles;
	U32* meshIndexCounts;
	U32 meshCount;
	IndexedVertexBuffer indexedVertexBuffer;
};


struct ModelData {
	MeshData meshData;
	uint32_t meshCount;
	uint32_t materialCount;
	std::vector<U32> meshIndexCounts;
//	std::vector<MaterialInfo> materialInfos;
	std::string materialName;
};

void ImportModelData (ModelData* data, const std::string& filename);

void SaveModelData(ModelData* data, const std::string& filename);
void LoadModelData(ModelData* data, const std::string& filename);


Model CreateModel (ModelData* data);

// Model* ImportModel(const std::string& filename);