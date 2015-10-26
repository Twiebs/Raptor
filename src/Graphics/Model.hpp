#pragma once

struct MaterialHandle;

struct Model {
	MaterialHandle* meshMaterialHandles;
	U32* meshIndexCounts;
	U32 meshCount;
	IndexedVertexBuffer indexedVertexBuffer;
};

Model* ImportModel(const std::string& filename);