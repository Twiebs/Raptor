#include <Core/types.h>

#include <Graphics/Mesh.hpp>

#include <mutex>

#include <Engine/Assets.hpp>
#include <Math/Geometry2D.hpp>

struct TerrainManager {
	std::vector<MaterialHandle> materials;
	U32 materialCount;

	std::vector<Mesh> terrainMeshes;
	U32 terrainCount;
	std::vector<GLuint> alphaMaps; // materialCount per terrainMesh

	U32 managerMaxWidth;
	U32 managerMaxLength;
	U32 managerArea;

	float chunkWidth;
	float chunkLength;
	float chunkArea;
	U32 chunkResolution;
	U32 chunkCellsPerTexcoord;

	ShaderHandle shaderHandle;

	TerrainManager(U32 materialCount, U32 max_width, U32 max_length, float terrainWidth, float terrainLength, U32 terrainResolution, U32 cellsPerTexcoord);
	~TerrainManager();

	U32 GetTerrainIndex(U32 xcoord, U32 ycoord);
	Rectangle GetBoundingRectangle();

	void draw();
};

struct TerrainStreamer {
	TerrainManager terrainManager;
	std::vector<U8*> maskDatas;
	std::vector<MeshData> meshDatas;
	
	std::vector<Vector3> chunksToStream;
	std::vector<bool> terrainTaskAvaible;

	std::function<U8(float, float, U32)> materialMaskProc;
	std::function<float(float, float)> heightMapProc;

	float currentXOrigin;
	float currentZOrigin;

	TerrainStreamer(U32 materialCount, U32 max_width, U32 max_length, float terrainWidth, float terrainLength, U32 terrainResolution, U32 cellsPerTexcoord);
	~TerrainStreamer();

	void update(const Vector3& position);
};