
#include <Core/Platform.h>

#include <Engine/GFX2D.hpp>
#include <Engine/ECSManager.hpp>

#include <Math/Random.hpp>
#include <Graphics/ModelData.hpp>
#include <Engine/GFX3D.hpp>
#include <Graphics/GLSLProgram.hpp>


#include <Graphics/Mesh.hpp>
#include <Math/Procedural3D.hpp>

#include <Graphics/Render3D.hpp>
#include <Graphics/imgui.h>
#include <Graphics/Lighting.hpp>
#include <Math/Noise.hpp>
#include <Graphics/GLRenderer.hpp>

#include <Utils/utils.hpp>

using namespace Raptor;

#include <thread>
#include <mutex>
#include <condition_variable>

#ifndef ASSET_DIRECTORY
#define ASSET_DIRECTORY "../../resources/assets/"
#endif

#ifndef SHADER_DIRECTORY
#define SHADER_DIRECTORY "../../resources/shaders/"
#endif

#define ASSET_FILE(filename) ASSET_DIRECTORY filename
#define SHADER_FILE(filename) SHADER_DIRECTORY filename


void DrawMesh (MeshData* data, MeshVertexBuffer* buffer) {
	glBindVertexArray(buffer->vertexArrayID);
	glDrawElements(GL_TRIANGLES, data->indexCount, GL_UNSIGNED_INT, 0);
}

void ShowCameraDebug (const Camera& camera) {
	ImGui::Begin("Camera Debug Info");
	ImGui::Text("Position: %f, %f, %f", camera.position.x, camera.position.y, camera.position.z);
	ImGui::Text("Yaw: %f", camera.yaw);
	ImGui::Text("Pitch %f", camera.pitch);
	ImGui::Text("Viewport: %f, %f", camera.viewportWidth, camera.viewportHeight);
	ImGui::End();
}

void ShowLightParamaters (const DirectionalLight& light) {
	ImGui::Begin("Light Paramaters");
	ImGui::SliderFloat3("Direction", (float*)&light.direction, -1.0f, 1.0f);
	ImGui::ColorEdit3("Color", (float*)&light.color.x);
	ImGui::SliderFloat("Ambient", (float*)&light.ambient, 0.0f, 1.0f);
	ImGui::End();
}

void BeginWireframe() {
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void EndWireframe() {
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}


std::vector<Material> g_materials(25);
const Material& GetMaterial (U32 materialID) {
	return g_materials[materialID];
};

U32 LoadMaterial (const std::string& diffuseFilename, const std::string& specularFilename, const std::string& normalFilename) {
	g_materials.push_back(Material());
	auto materialID = g_materials.size() - 1;
	auto& material = g_materials.back();
	material.diffuseMapID 	= LoadTextureReleaseData(diffuseFilename, GL_REPEAT);
	material.specularMapID  = LoadTextureReleaseData(specularFilename, GL_REPEAT);
	material.normalMapID 	= LoadTextureReleaseData(normalFilename, GL_REPEAT);
	return materialID;
}

void DrawMesh (const Mesh& mesh) {
	glBindVertexArray(mesh.vertexArrayID);
	// auto& material = GetMaterial(mesh.materialID);
	// BindMaterial(material);
	glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, 0);
}

//struct Material {
//	GLuint textureArrayID;
//	U32 usedTextureFlags;
//};


struct TerrainManager {
	std::vector<U32> materials;
	U32 materialCount;

	std::vector<Mesh> terrainMeshes;
	U32 terrainCount;
	std::vector<GLuint> alphaMaps; // materialCount per terrainMesh

	U32 managerMaxWidth;
	U32 managerMaxLength;
	U32 managerArea;

	float terrainWidth;
	float terrainLength;
	float terrainArea;

	U32 terrainResolution;
	U32 terrainCellsPerTexcoord;

	GLuint shaderID;

	TerrainManager(U32 materialCount, U32 max_width, U32 max_length, float terrainWidth, float terrainLength, U32 terrainResolution, U32 cellsPerTexcoord);
	~TerrainManager();

	void draw();
};



struct Work {
	std::function<void()> execute;
	std::function<void()> finalize;
};

struct WorkQueue {
	std::mutex mutex;
	std::condition_variable condition_variable;
	
	std::vector<Work> workToExecute;
	std::vector<Work> workToFinalize;
};

struct TaskManager {
	std::vector<std::thread> threads;
	WorkQueue workQueue;
};

global_variable TaskManager g_TaskManager;


static void ThreadProc(WorkQueue* queue) {
	bool isRunning = true;

	Work work;
	bool workNeedsDoing = false;

	while (isRunning) {
		if (workNeedsDoing) {
			work.execute();
			workNeedsDoing = false;

			queue->mutex.lock();
			queue->workToFinalize.push_back(work);
			queue->mutex.unlock();
		}


		std::unique_lock<std::mutex> lock(queue->mutex);
		queue->condition_variable.wait(lock);

		if (queue->workToExecute.size() > 0) {
			work = queue->workToExecute.back();
			queue->workToExecute.pop_back();
			workNeedsDoing = true;
		}
	}
}

void InitTaskManager (TaskManager* manager) {
	auto threadCount = std::thread::hardware_concurrency() - 1;
	manager->threads.resize(threadCount);

	for (U32 i = 0; i < threadCount; i++) {
		manager->threads[i] = std::thread(ThreadProc, &manager->workQueue);
	}
}

void InitTaskManager() {
	InitTaskManager(&g_TaskManager);
}

// DONT SUBMIT NEW WORK IN A FINALIZER!
void SubmitTask (std::function<void()> execute, std::function<void()> finalize) {
	WorkQueue* queue = &g_TaskManager.workQueue;

	queue->mutex.lock();
	queue->workToExecute.push_back({ execute, finalize });
	queue->condition_variable.notify_one();
	queue->mutex.unlock();
}



void FinializeCompletedTasks() {
	WorkQueue* queue = &g_TaskManager.workQueue;

	queue->mutex.lock();
	for (U32 i = 0; i < queue->workToFinalize.size(); i++) {
		queue->workToFinalize[i].finalize();
	}

	queue->workToFinalize.clear();
	queue->mutex.unlock();
}

TerrainManager::TerrainManager (U32 material_count, U32 max_width, U32 max_length, float terrainWidth, float terrainLength, U32 terrain_resolution, U32 cellsPerTexcoord) :
	terrainWidth(terrainWidth),
	terrainLength(terrainLength),
	terrainResolution(terrain_resolution),
	terrainCellsPerTexcoord(cellsPerTexcoord)
{
	materialCount = material_count;
	managerMaxWidth = max_width;
	managerMaxLength = max_length;
	managerArea = max_width * max_length;

	materials.resize(materialCount);

	terrainCount = managerArea;
	alphaMaps.resize(materialCount * terrainCount);
	terrainMeshes.resize(managerArea);

	GLSLCompiler compiler;
	compiler.addSourceFile(VERTEX_SHADER,	SHADER_FILE("terrain.vert"));
	compiler.addSourceFile(FRAGMENT_SHADER, SHADER_FILE("terrain.frag"));
	compiler.addDefnition("#define MATERIAL_COUNT " + std::to_string(materialCount) + "\n");
	compiler.addDefnition("#define TERRAIN_WIDTH " + std::to_string(terrainWidth) + "\n");
	compiler.addDefnition("#define TERRAIN_LENGTH " + std::to_string(terrainLength) + "\n");
	compiler.addDefnition("#define TERRAIN_CELLS_PER_TEXCOORD " + std::to_string(terrainCellsPerTexcoord) + "\n");
	shaderID = compiler.compile();
}

TerrainManager::~TerrainManager() {
	glDeleteProgram(shaderID);
}


inline void BindTexture2DToUnit (GLuint textureID, U32 unitIndex) {
	glActiveTexture(GL_TEXTURE0 + unitIndex);
	glBindTexture(GL_TEXTURE_2D, textureID);
}

void TerrainManager::draw() {

	auto BindAlphaMaps = [&](U32 terrainChunkIndex) {
		for (U32 i = 0; i < materialCount; i++) {
			BindTexture2DToUnit(alphaMaps[(terrainChunkIndex * materialCount) + i], materialCount + i);
		}
	};

	for (U32 i = 0; i < materialCount; i++) {
		auto& material = GetMaterial(materials[i]);
		BindTexture2DToUnit(material.diffuseMapID, (materialCount * 0) + i);
	}

	for (U32 i = 0; i < managerArea; i++) {
		BindAlphaMaps(i);
		auto& terrainMesh = terrainMeshes[i];
		glBindVertexArray(terrainMesh.vertexArrayID);
		glDrawElements(GL_TRIANGLES, terrainMesh.indexCount, GL_UNSIGNED_INT, 0);
	}
}

struct TerrainStreamer {
    TerrainManager terrainManager;
    std::vector<U8*> maskDatas;
    std::vector<MeshData> meshDatas;

    TerrainStreamer(U32 materialCount, U32 max_width, U32 max_length, float terrainWidth, float terrainLength, U32 terrainResolution, U32 cellsPerTexcoord);
};

TerrainStreamer::TerrainStreamer(U32 materialCount, U32 max_width, U32 max_length, float terrainWidth, float terrainLength, U32 terrainResolution, U32 cellsPerTexcoord) :
    terrainManager(materialCount, max_width, max_length, terrainWidth, terrainLength, terrainResolution, cellsPerTexcoord) {
}


void ShowTerrainProperties (const TerrainManager& manager) {
	static GLint debugAlphaMapIndexLocation = GetUniformLocation(manager.shaderID, "debugAlphaMapIndex");

	ImGui::Begin("Terrain Manager Properties");
	for (auto i = 0; i < manager.materialCount; i++) {
		ImGui::Text("Material: %d", i);
		ImGui::SameLine();
		ImGui::PushID(i);
		if (ImGui::Button("Show Alpha Map")) {
			GLint last_program;
			glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
			glUseProgram(manager.shaderID);
			glUniform1i(debugAlphaMapIndexLocation, i);
			glUseProgram(last_program);
		}
		ImGui::PopID();
	}

	if (ImGui::Button("Clear Alpha Map")) {
		GLint last_program;
		glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
		glUseProgram(manager.shaderID);
		glUniform1i(debugAlphaMapIndexLocation, -1);
		glUseProgram(last_program);
	}

	ImGui::End();
}


void RunBasicGLTest() {
	PlatformCreate("Basic GLTest");
	// auto shaderID = CreateShader("shaders/terrain.vert", "shaders/terrain.frag");

	InitTaskManager();

	Camera camera(Vector3(20.0f, 5.0f, 200.0f), 1280.0f, 720.0f);
	camera.farClip = 10000.0f;

	DirectionalLight directionalLight;
	// UniformDirectionalLight(shaderID, 0, directionalLight);

	// TODO action bar style terrain placer thingy

	SubmitTask([&directionalLight]() {
		std::cout << "Started A Task\n";
		directionalLight.color = Vector3(1.0f, 0.0f, 0.0f);
	}, 

	[]() {
		std::cout << "Finished a task\n";
	});

	static const int WORLD_WIDTH = 2;
	static const int WORLD_HEIGHT = 2;
	static const int WORLD_AREA = WORLD_WIDTH * WORLD_HEIGHT;
	static const float TERRAIN_WIDTH = 128.0f;
	static const float TERRAIN_HEIGHT = 128.0f;
	static const int TERRAIN_RESOLUTION = 128;
	static const int TERRAIN_MATERIAL_COUNT = 2;

	TerrainManager terrain_manager(TERRAIN_MATERIAL_COUNT, WORLD_WIDTH, WORLD_HEIGHT, TERRAIN_WIDTH, TERRAIN_HEIGHT, TERRAIN_RESOLUTION, 4);

	U8* alphaData = (U8*)malloc(TERRAIN_RESOLUTION * TERRAIN_RESOLUTION);
	memset(alphaData, 255, TERRAIN_RESOLUTION * TERRAIN_RESOLUTION);
	terrain_manager.alphaMaps[0] = CreateAlphaMap(TERRAIN_RESOLUTION, TERRAIN_RESOLUTION, alphaData);
//
//	for (U32 i = 0; i < terrain_manager.terrainCount; i++) {
//		terrain_manager.alphaMaps[i * terrain_manager.materialCount] = CreateAlphaMap(terrain_manager.terrainResolution, terrain_manager.terrainResolution, alphaData);
//	}

	OpenSimplexNoise materialNoise(567657);
	auto MaterialMaskGenerationProc = [&materialNoise](float x, float y, U32 materialIndex) -> U8 {
		if (materialIndex == 0) return 255;

		auto value = materialNoise.FBM(x, y, 1, 0.1f, 0.5f);
		value = (value + 1.0f) / 2.0f;
		return (U8)(value * 255);
	};


	for (U32 i = 0; i < terrain_manager.terrainCount; i++) {
		U32 terrainXIndex = i % terrain_manager.managerMaxWidth;
		U32 terrainZIndex = i / terrain_manager.managerMaxWidth;
		float terrainWorldX = terrainXIndex * (terrain_manager.terrainWidth - 1);
		float terrainWorldZ = terrainZIndex * (terrain_manager.terrainLength - 1);
		for (U32 n = 0; n < terrain_manager.materialCount; n++) {
			for (U32 z = 0; z < terrain_manager.terrainResolution; z++) {
				for (U32 x = 0; x < terrain_manager.terrainResolution; x++) {
					U32 alphaIndex = (z * terrain_manager.terrainResolution) + x;
					auto xCoord = x + terrainWorldX;
					auto zCoord = z + terrainWorldZ;
					alphaData[alphaIndex] = MaterialMaskGenerationProc(xCoord, zCoord, n);
				}
			}
			terrain_manager.alphaMaps[(i * terrain_manager.materialCount) + n] = CreateAlphaMap(terrain_manager.terrainResolution, terrain_manager.terrainResolution, alphaData);
		}
	}

	free(alphaData);

	MeshData terrainDatas[WORLD_AREA];
	for (U32 i = 0; i < WORLD_AREA; i++) {
		auto& terrainMesh = terrain_manager.terrainMeshes[i];
		auto& terrainData = terrainDatas[i];
		auto terrainX = (i % WORLD_WIDTH) * (TERRAIN_WIDTH - 1);
		auto terrainY = (i / WORLD_WIDTH) * (TERRAIN_HEIGHT - 1);

		OpenSimplexNoise simplexNoise0(234324);
		OpenSimplexNoise simplexNoise1(4564564);
		CreatePlaneMesh(&terrainData, terrainX, terrainY, TERRAIN_WIDTH, TERRAIN_HEIGHT, TERRAIN_RESOLUTION, 4, [&simplexNoise0, &simplexNoise1](float x, float y) {
			auto noise0 = simplexNoise0.FBM(x, y, 6, 0.005f, 0.5f);
			auto noise1 = simplexNoise1.RidgedNoise(x, y, 6, 0.0009, 0.25);
			return (noise0 + noise1) * 80;
		});

		InitMesh(&terrainMesh, &terrainData);
	}

	terrain_manager.materials[0] = LoadMaterial(ASSET_FILE("materials/dirt0/diffuse.tga"),  ASSET_FILE("materials/toon_stone0/diffuse.tga"), ASSET_FILE("materials/smooth_stone/diffuse.tga"));
	terrain_manager.materials[1] = LoadMaterial(ASSET_FILE("materials/grass0/diffuse.tga"), ASSET_FILE("materials/toon_stone0/diffuse.tga"), ASSET_FILE("materials/smooth_stone/diffuse.tga"));
	//terrain_manager.materials[2] = LoadMaterial("assets/materials/smooth_stone/diffuse.tga", "assets/materials/toon_stone0/diffuse.tga", "assets/materials/smooth_stone/diffuse.tga");


	ImGui::Init();

	GLCheckErrors();
	PlatformRun([&](double deltaTime) -> void {
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(terrain_manager.shaderID);

		if (PlatformGetButton(MOUSE_BUTTON_RIGHT)) FPSCameraControlUpdate(&camera);


		UniformDirectionalLight(terrain_manager.shaderID, 0, directionalLight);

		UpdateCamera(&camera);
		SetCameraUniforms(camera);
		SetMatrix4Uniform(MODEL_MATRIX_LOCATION, Matrix4::Translate(0.0f, 0.0f, -1.0f));
		SetMatrix4Uniform(VIEW_MATRIX_LOCATION, camera.view);
		SetMatrix4Uniform(PROJECTION_MATRIX_LOCATION, camera.projection);

		glEnable(GL_BLEND);
		// glBlendFunc()


		// BeginWireframe();
		terrain_manager.draw();
		// EndWireframe();

		ImGui::BeginFrame();
		ShowCameraDebug(camera);
		ShowLightParamaters(directionalLight);
		ShowTerrainProperties(terrain_manager);
		ImGui::EndFrame();

		FinializeCompletedTasks();
	});
}

int main() {
	RunBasicGLTest();
}
