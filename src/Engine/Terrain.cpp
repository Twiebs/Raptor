#include <Engine/Engine.hpp>
#include <Engine/Assets.hpp>
#include <Engine/Terrain.hpp>
#include <Engine/GFX3D.hpp>

#include <Graphics/GLRenderer.hpp>
#include <Graphics/GLSLProgram.hpp>

#include <Core/Tasks.hpp>

#include <Math/Procedural3D.hpp>
#include <Math/Noise.hpp>

TerrainManager::TerrainManager(U32 material_count, U32 max_width, U32 max_length, float terrainWidth, float terrainLength, U32 terrain_resolution, U32 cellsPerTexcoord) :
	chunkWidth(terrainWidth),
	chunkLength(terrainLength),
	chunkResolution(terrain_resolution),
	chunkCellsPerTexcoord(cellsPerTexcoord)
{
	materialCount = material_count;
	managerMaxWidth = max_width;
	managerMaxLength = max_length;
	managerArea = max_width * max_length;

	materials.resize(materialCount);

	terrainCount = managerArea;
	alphaMaps.resize(materialCount * terrainCount);
	terrainMeshes.resize(managerArea);

	BeginShaderBuilder("main_terrain");
	AddShaderSourceFile(VERTEX_SHADER, SHADER_FILE("terrain.vert"));
	AddShaderSourceFile(FRAGMENT_SHADER, SHADER_FILE("terrain.frag"));
	AddShaderString("#define MATERIAL_COUNT " + std::to_string(materialCount) + "\n");
	AddShaderString("#define TERRAIN_WIDTH " + std::to_string(terrainWidth) + "\n");
	AddShaderString("#define TERRAIN_LENGTH " + std::to_string(terrainLength) + "\n");
	AddShaderString("#define TERRAIN_CELLS_PER_TEXCOORD " + std::to_string(chunkCellsPerTexcoord) + "\n");
	shaderHandle = LoadShaderFromBuilder();
}

TerrainManager::~TerrainManager() {
	// XXX
	// TODO NOTE shaders and other various assets are no longer deleted properly!
}

struct TerrainDrawCommand {
	
	Mesh* terrainMesh;
};

void TerrainManager::draw() {
	auto& shader = GetShader(shaderHandle);
	GFX3D::Begin(shader);

	auto BindAlphaMaps = [&](U32 terrainChunkIndex) {
		for (U32 i = 0; i < materialCount; i++) {
			BindTexture2DToUnit(alphaMaps[(terrainChunkIndex * materialCount) + i], (materialCount * 3) + i);
		}
	};

	for (U32 i = 0; i < materialCount; i++) {
		auto& material = GetMaterial(materials[i]);
		BindTexture2DToUnit(material.diffuseMapID, (materialCount * 0) + i);
		BindTexture2DToUnit(material.specularMapID, (materialCount * 1) + i);
		BindTexture2DToUnit(material.normalMapID, (materialCount * 2) + i);
	}

	for (U32 i = 0; i < managerArea; i++) {
		BindAlphaMaps(i);
		auto& terrainMesh = terrainMeshes[i];
		// GFX3D::DrawImmediately(terrainMesh);
		//glBindVertexArray(terrainMesh.vertexArrayID);
		//glDrawElements(GL_TRIANGLES, terrainMesh.indexCount, GL_UNSIGNED_INT, 0);
	}

	GFX3D::End(shader);
}

Rectangle TerrainManager::GetBoundingRectangle() {
	return Rectangle(0.0f, 0.0f, chunkWidth * managerMaxWidth, chunkLength * managerMaxLength);
}

struct TerrainGenerationTask : public ITask {
	TerrainGenerationTask(TerrainStreamer* terrainStreamer, float x, float y, U32 terrainIndex);
	void execute(U32 workerID) override;
	void finalize(U32 workerID) override;
	TerrainStreamer* terrainStreamer;
	U32 terrainIndex;
	float x, y;
};

// TODO cells per UV coord should be somthing that is specified in the shader
// on a per material basis!

TerrainGenerationTask::TerrainGenerationTask(TerrainStreamer* terrainStreamer, float x, float y, U32 terrainIndex)
	: terrainStreamer(terrainStreamer), x(x), y(y), terrainIndex(terrainIndex) { }

// XXX remove simplexNoise and put it in the terrainStreamer
#include <Utils/Profiler.hpp>
void TerrainGenerationTask::execute(U32 workerID) {
	auto terrainData = &terrainStreamer->meshDatas[workerID];
	auto& terrainManager = terrainStreamer->terrainManager;
	OpenSimplexNoise simplexNoise0(234324);
	OpenSimplexNoise simplexNoise1(4564564);

	GeneratePlaneMesh(terrainData, x, y, terrainManager.chunkWidth, terrainManager.chunkLength, terrainManager.chunkResolution, 4, [&simplexNoise0, &simplexNoise1, &terrainManager](float x, float y) {
		static const float roughness = 0.3f;
		static const float freq0 = (1.0f / terrainManager.chunkWidth) * roughness;
		static const float freq1 = (1.0f / terrainManager.chunkWidth) * roughness;

		// TODO fixnoise
		auto influence = (float)simplexNoise0.FBM(x, y, 6, 0.001, 0.5f);
		auto noise0 = simplexNoise0.FBM(x, y, 6, freq0, 0.5f);
		auto noise1 = (simplexNoise1.RidgedNoise(x, y, 6, freq1, 0.5) * influence) * 3.0f;
		return (float)0;
	});
}

void TerrainGenerationTask::finalize(U32 workerID) {
	auto data = &terrainStreamer->meshDatas[workerID];
	auto mesh = &terrainStreamer->terrainManager.terrainMeshes[terrainIndex];
	InitMesh(mesh, data);
	terrainStreamer->terrainTaskAvaible[workerID] = true;
}

TerrainStreamer::TerrainStreamer (U32 materialCount, U32 max_width, U32 max_length, float terrainWidth, float terrainLength, U32 terrainResolution, U32 cellsPerTexcoord) :
	terrainManager(materialCount, max_width, max_length, terrainWidth, terrainLength, terrainResolution, cellsPerTexcoord) {
	maskDatas.resize(GetTaskWorkerCount());
	meshDatas.resize(GetTaskWorkerCount());
	terrainTaskAvaible.resize(GetTaskWorkerCount(), true);

	currentXOrigin = 0.0f;
	currentZOrigin = 0.0f;

	for (U32 i = 0; i < terrainManager.managerArea; i++) {
		auto terrainX = (i % terrainManager.managerMaxWidth) * (terrainManager.chunkWidth - 1);
		auto terrainY = (i / terrainManager.managerMaxWidth) * (terrainManager.chunkLength - 1);
		chunksToStream.emplace_back(terrainX, terrainY, (float)i); // TODO replace chunks to stream!
	}

	//for (U32 i = 0; i < terrainManager.terrainCount; i++) {
	//	U32 terrainXIndex = i % terrainManager.managerMaxWidth;
	//	U32 terrainZIndex = i / terrainManager.managerMaxWidth;
	//	float terrainWorldX = terrainXIndex * (terrainManager.chunkWidth - 1);
	//	float terrainWorldZ = terrainZIndex * (terrainManager.chunkLength - 1);
	//	for (U32 n = 0; n < terrainManager.materialCount; n++) {
	//		for (U32 z = 0; z < terrainManager.chunkResolution; z++) {
	//			for (U32 x = 0; x < terrainManager.chunkResolution; x++) {
	//				U32 alphaIndex = (z * terrainManager.chunkResolution) + x;
	//				auto xCoord = x + terrainWorldX;
	//				auto zCoord = z + terrainWorldZ;
	//				alphaData[alphaIndex] = MaterialMaskGenerationProc(xCoord, zCoord, n);
	//			}
	//		}
	//		terrain_manager.alphaMaps[(i * terrain_manager.materialCount) + n] = CreateAlphaMap(terrain_manager.chunkResolution, terrain_manager.chunkResolution, alphaData);
	//	}
	//}
	//

	U32 vertexCount, indexCount;
	GetPlaneMeshMemorySize(terrainResolution, &vertexCount, &indexCount);

	for (U32 i = 0; i < GetTaskWorkerCount(); i++) {
		AllocateMeshData(&meshDatas[i], vertexCount, indexCount);
	}
}

TerrainStreamer::~TerrainStreamer() {
	for (U32 i = 0; i < GetTaskWorkerCount(); i++) {
		FreeMeshData(&meshDatas[i]);
	}
}

void TerrainStreamer::update(const Vector3& position) {
	auto centerXPos = currentXOrigin + terrainManager.chunkWidth  * (terrainManager.managerMaxWidth*0.5f);
	auto centerZPos = currentZOrigin + terrainManager.chunkLength * (terrainManager.managerMaxLength*0.5f);
	auto xdiff = position.x - centerXPos;
	auto zdiff = position.z - centerZPos;

	//if (abs(xdiff) > terrainManager.chunkWidth) {
	//	if (xdiff > 0.0f) {
	//		for (U32 i = 0; i < terrainManager.managerMaxLength; i++) {
	//			terrainManager.GetTerrainIndex(0, i);
	//			chunksToGenerate.push_back();


	//		}
	//	}


	//}

	if (abs(zdiff) > terrainManager.chunkLength) {

	}



	U32 maxTaskSubmissionCount = 0;
	for (auto& avaible : terrainTaskAvaible) {
		if (avaible) {
			maxTaskSubmissionCount++;
			avaible = false;
		}
	}

	if (maxTaskSubmissionCount == 0) return;

	for (U32 i = 0; i < maxTaskSubmissionCount; i++) {
		if (chunksToStream.size() == 0) return;
		auto chunk_info = chunksToStream.back();
		chunksToStream.pop_back();
		Engine::ScheduleTask<TerrainGenerationTask>(this, chunk_info.x, chunk_info.y, (U32)chunk_info.z);
	}
}