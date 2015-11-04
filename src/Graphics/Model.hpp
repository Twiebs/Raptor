#pragma once

struct MaterialHandle;

#include <Graphics/Material.hpp>

typedef std::string AssetFileIdentifer;
struct MaterialTextureMapInfo {
	AssetFileIdentifer name;
};

struct Model {
	MaterialHandle* meshMaterialHandles;
	U32* meshIndexCounts;
	U32 meshCount;
	IndexedVertexBuffer indexedVertexBuffer;
};


// Going to do stupid things for now!
// For the win my friend!
struct ModelData {
	MeshData* meshData;
	U32 meshCount;
	std::vector<U32> meshIndexCounts;
	std::vector<MaterialAssetInfo> materialInfos;
};

void ImportModelData (ModelData* data, const std::string& filename);

Model CreateModel (ModelData* data);

// Model* ImportModel(const std::string& filename);