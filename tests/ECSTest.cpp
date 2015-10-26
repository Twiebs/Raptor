#include <Core/Platform.h>
#include <Core/Tasks.hpp>

#include <Engine/Assets.hpp>
#include <Engine/Debug.hpp>
#include <Engine/GFX2D.hpp>
#include <Engine/GFX3D.hpp>
#include <Engine/ECSManager.hpp>
#include <Engine/Terrain.hpp>

#include <Math/Random.hpp>
#include <Graphics/ModelData.hpp>

#include <Graphics/GLSLProgram.hpp>

#include <Graphics/Mesh.hpp>
#include <Math/Procedural3D.hpp>

#include <Graphics/Render3D.hpp>
#include <Graphics/imgui.h>
#include <Graphics/Liquid.hpp>
#include <Graphics/Lighting.hpp>
#include <Math/Noise.hpp>
#include <Graphics/GLRenderer.hpp>

#include <Utils/utils.hpp>
#include <Utils/Profiler.hpp>

using namespace Raptor;

#include <Core/config.h>

void ShowAssetBrowser () {
	ImGui::Begin("Asset Browser");
	static S32 selectedIndex = -1;
	static int currentAssetType = 0;

	if (ImGui::Button("Materials")) { 
		currentAssetType = 0; 
		selectedIndex = -1;
	}

	ImGui::SameLine();
	if (ImGui::Button("Shaders")) {
		currentAssetType = 1; 
		selectedIndex = -1;
	}

	auto& manifest = GetGlobalAssetManifest();
	ImGui::BeginChild("AssetList", ImVec2(200.0f, 0.0f), true);
	if (currentAssetType == 0) {
		for (U32 i = 0; i < manifest.materials.size(); i++) {
			const auto& info = manifest.materialAssetInfos[i];
			if (ImGui::Button(info.name.c_str(), ImVec2(200.0f, 0.0f))) {
				selectedIndex = i;
			}
		}
	} else {
		for (U32 i = 0; i < manifest.shaderPrograms.size(); i++) {
			const auto& data = manifest.shaderBuilderData[i];
			if (ImGui::Button(data.name.c_str(), ImVec2(200.0f, 0.0f))) {
				selectedIndex = i;
			}
		}
	}

	
	ImGui::EndChild();

	ImGui::SameLine();
	ImGui::BeginChild("AssetInspector", ImVec2(0, 0), true);
	if (selectedIndex >= 0) {
		if (currentAssetType == 0) {
			auto& material = manifest.materials[selectedIndex];
			ImGui::Image((ImTextureID)material.diffuseMapID, ImVec2(128.0f, 128.0f));
			ImGui::SameLine();
			ImGui::Image((ImTextureID)material.specularMapID, ImVec2(128.0f, 128.0f));
			ImGui::SameLine();
			ImGui::Image((ImTextureID)material.normalMapID, ImVec2(128.0f, 128.0f));
			if (ImGui::Button("Reload Material")) {
				LOG_WARNING("Material reloading not currently implemented!");
			}

		} else {
			if (ImGui::Button("Reload Shader")) {
				ReloadShader(ShaderHandle{(U32)selectedIndex});
			}
		}
	}

	ImGui::EndChild();
	
	ImGui::End();
}


static void CreateTestMaterials() {
	MaterialData data;
	data.diffuseTextureFilename = ASSET_FILE("materials/dirt0/diffuse.tga");
	data.normalTextureFilename  = ASSET_FILE("materials/dirt0/normal.tga");
	SaveMaterialData(data, "dirt.material");

	data.diffuseTextureFilename = ASSET_FILE("materials/grass0/diffuse.tga");
	data.normalTextureFilename = ASSET_FILE("materials/grass0/normal.tga");
	SaveMaterialData(data, "grass.material");
}

void ShowMaterialEditor() {
	ImGui::Begin("Material Editor");
	static bool newMaterialWindowActive = false;
	if (ImGui::Button("Create New Material")) newMaterialWindowActive = true;
	if (newMaterialWindowActive) {
		static char material_name_buffer[MATERIAL_NAME_MAX_SIZE];
		newMaterialWindowActive = false;
	}
	ImGui::End();
}

//void ShowShaderManifest() {
//#ifdef GENERATE_SHADER_SOURCE_FILES
//	auto manifest = GetGlobalShaderManifest();
//	ImGui::Begin("ShaderManifest");
//
//	
//	static S32 selectedIndex = -1;
//	ImGui::BeginChild("ShaderList", ImVec2(200.0f, 0.0f), true);
//	for (U32 i = 0; i < manifest.shaderPrograms.size(); i++) {
//		const auto& info = manifest.compileInfos[i];
//		if (ImGui::Button(info.name.c_str(), ImVec2(200.0f, 0.0f))) {
//			selectedIndex = i;
//		}
//	}
//
//	ImGui::EndChild();
//	ImGui::SameLine();
//	if (selectedIndex >= 0) {
//		if (ImGui::Button("Reload")) {
//			RecompileShader(ShaderHandle{(U32)selectedIndex});
//		}
//	}
//	
//	
//	ImGui::End();
//#endif
//}

struct BasicEntity {
	Vector3 position;
	ModelHandle& modelHandle;
};

std::vector<BasicEntity> g_Entities;

template <typename TEntity>
struct EntityManager {
	std::vector<TEntity> entities;
};

void ShowDebugRenderSettings (DebugRenderSettings* settings) {
	ImGui::Begin("Debug Render Settings");
	ImGui::Checkbox("Disable Normal Mapping", &settings->disableNormalMaps);
	ImGui::Checkbox("Enable Wireframe", &settings->enableWireframe);

	//for (auto i = 0; i < manager.materialCount; i++) {
	//	ImGui::Text("Material: %d", i);
	//	ImGui::SameLine();
	//	ImGui::PushID(i);
	//	if (ImGui::Button("Show Alpha Map")) {
	//		GLint last_program;
	//		glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
	//		BindShader(shader);
	//		glUniform1i(debugAlphaMapIndexLocation, i);
	//		glUseProgram(last_program);
	//	}
	//	ImGui::PopID();
	//}

	//if (ImGui::Button("Clear Alpha Map")) {
	//	GLint last_program;
	//	glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
	//	BindShader(manager.shaderID);
	//	glUniform1i(debugAlphaMapIndexLocation, -1);
	//	glUseProgram(last_program);
	//}

	ImGui::End();
}

struct AtmoshpericFog {
	float density;
	float gradient;
};


static void ShowProfilerGUI (Profiler* profiler) {
	ImGui::Begin("Profiler");
	ImGui::BeginChild("PersistantEntries", ImVec2(400.0f, 0.0f), true);
	for (U32 i = 0; i < profiler->activePersistantEntries.size(); i++) {
		auto entryIndex = profiler->activePersistantEntries[i];
		auto entry = &profiler->persistantEntries[entryIndex];
		if (ImGui::CollapsingHeader(entry->name.c_str())) {
			auto timeIndex = entry->writeIndex - 1;
			if (timeIndex < 0) timeIndex = PROFILER_PERSISTANT_SAMPLE_COUNT;
			ImGui::Text("Elapsed Time: %f ms", entry->elapsedTimes[timeIndex]);
			ImGui::Text("Elapsed Cycles: %d", entry->elapsedCycles[timeIndex]);
			ImGui::PushID(i);
			ImGui::PlotLines("", entry->elapsedTimes, PROFILER_PERSISTANT_SAMPLE_COUNT, timeIndex);
			ImGui::PopID();
		}
	}
	ImGui::EndChild();
	ImGui::SameLine();
	ImGui::BeginChild("InstantaneousEntries", ImVec2(250.0f, 0.0f), true);
	for (U32 i = 0; i < profiler->newEntryIndex; i++) {
		auto& entry = profiler->entires[i];
		ImGui::BeginChildFrame(i, ImVec2(235.0f, 50.0f));
		ImGui::Text(entry.name.c_str());
		ImGui::Text("Elapsed Time: %f ms", entry.elapsedTime);
		ImGui::Text("Elapsed Cycles: %d", entry.elapsedCycles);
		ImGui::EndChildFrame();
	}
	
	ImGui::EndChild();
	ImGui::End();
}

void ShowErrorMessageBox() {
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 0.9f));
	ImGui::SetNextWindowSize(ImVec2(300.0f, 60.0f));
	ImGui::SetNextWindowPos(ImVec2(8.0f, 8.0f));
	ImGui::Begin("ErrorMessage", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
	ImGui::TextColored(ImVec4(1.0, 0.0, 0.0, 1.0), "[ERROR] This is an error!");
	ImGui::End();
	ImGui::PopStyleColor();
}


void RunBasicGLTest() {
	PlatformCreate("Basic GLTest");
	// auto shaderID = CreateShader("shaders/terrain.vert", "shaders/terrain.frag");

	InitTaskManager();

	// CreateTestMaterials();

	ProfilerBeginEntry("Load Models");
	auto rockModelHandle  = LoadModel(ASSET_FILE("models/rock0/Rock1.obj"));
	auto plantModelHandle = LoadModel(ASSET_FILE("models/palm_plant/palm_plant.obj"));
	ProfilerEndEntry("Load Models");

	ProfilerBeginEntry("Load Materials");
	auto dirtMaterialHandle	 = LoadMaterial("dirt.material");
	auto grassMaterialHandle = LoadMaterial("grass.material");
	ProfilerEndEntry("Load Materials");

	DirectionalLight directionalLight;
	AddLight(directionalLight);

	// UniformDirectionalLight(shaderID, 0, directionalLight);

	// TODO action bar style terrain placer thingy

	static const int WORLD_WIDTH = 9;
	static const int WORLD_HEIGHT = 9;
	static const int WORLD_AREA = WORLD_WIDTH * WORLD_HEIGHT;
	static const float TERRAIN_WIDTH = 128.0f;
	static const float TERRAIN_HEIGHT = 128.0f;
	static const int TERRAIN_RESOLUTION = 128;
	static const int TERRAIN_MATERIAL_COUNT = 2;

	Camera camera(Vector3(WORLD_WIDTH * 0.5f * TERRAIN_WIDTH, 80.0f, WORLD_HEIGHT * TERRAIN_HEIGHT * 0.5f), 1280.0f, 720.0f);
	camera.farClip = 10000.0f;

	TerrainStreamer terrainStreamer(TERRAIN_MATERIAL_COUNT, WORLD_WIDTH, WORLD_HEIGHT, TERRAIN_WIDTH, TERRAIN_HEIGHT, TERRAIN_RESOLUTION, 4);
	auto& terrain_manager = terrainStreamer.terrainManager;

	U8* alphaData = (U8*)malloc(TERRAIN_RESOLUTION * TERRAIN_RESOLUTION);
	memset(alphaData, 255, TERRAIN_RESOLUTION * TERRAIN_RESOLUTION);
	terrain_manager.alphaMaps[0] = CreateAlphaMap(TERRAIN_RESOLUTION, TERRAIN_RESOLUTION, alphaData);

	OpenSimplexNoise materialNoise(567657);
	auto MaterialMaskGenerationProc = [&materialNoise](float x, float y, U32 materialIndex) -> U8 {
		if (materialIndex == 0) return 255;

		auto value = materialNoise.FBM(x, y, 1, 0.1f, 0.5f);
		value = (value + 1.0f) / 2.0f;
		return (U8)(value * 255);
	};

	ProfilerBeginEntry("Generate Alpha Maps");
	for (U32 i = 0; i < terrain_manager.terrainCount; i++) {
		U32 terrainXIndex = i % terrain_manager.managerMaxWidth;
		U32 terrainZIndex = i / terrain_manager.managerMaxWidth;
		float terrainWorldX = terrainXIndex * (terrain_manager.chunkWidth - 1);
		float terrainWorldZ = terrainZIndex * (terrain_manager.chunkLength - 1);
		for (U32 n = 0; n < terrain_manager.materialCount; n++) {
			for (U32 z = 0; z < terrain_manager.chunkResolution; z++) {
				for (U32 x = 0; x < terrain_manager.chunkResolution; x++) {
					U32 alphaIndex = (z * terrain_manager.chunkResolution) + x;
					auto xCoord = x + terrainWorldX;
					auto zCoord = z + terrainWorldZ;
					alphaData[alphaIndex] = MaterialMaskGenerationProc(xCoord, zCoord, n);
				}
			}
			terrain_manager.alphaMaps[(i * terrain_manager.materialCount) + n] = CreateAlphaMap(terrain_manager.chunkResolution, terrain_manager.chunkResolution, alphaData);
		}
	}
	ProfilerEndEntry("Generate Alpha Maps");

	free(alphaData);

	terrain_manager.materials[0] = dirtMaterialHandle;
	terrain_manager.materials[1] = grassMaterialHandle;

	Mesh water_mesh;
	MeshData water_plane;
	CreatePlaneMesh(&water_plane, 0.0f, 0.0f, 64.0f, 64.0f, 512, 8);
	InitMesh(&water_mesh, &water_plane);

	ImGui::Init();

	FrameParameters params;
	params.clearColor = Vector3(0.53, 0.77, 0.92);

	Random rng;

	Rectangle terrainRect = terrain_manager.GetBoundingRectangle();
	for (U32 i = 0; i < 512; i++) {	
		Vector2 planeCoord = rng.PointInRectangle(terrainRect);
		g_Entities.emplace_back(BasicEntity { Vector3(planeCoord.x, 100.0f, planeCoord.y),  rockModelHandle });
	}

	Vector2 terrainCenter = GetCenter(terrainRect);

	ProfilerBeginEntry("CreateLiquidShader");
	BeginShaderBuilder("water_shader");
	AddShaderSourceFile(VERTEX_SHADER, SHADER_FILE("liquid.vert"));
	AddShaderSourceFile(FRAGMENT_SHADER, SHADER_FILE("liquid.frag"));
	auto liquidShaderHandle = LoadShaderFromBuilder();
	ProfilerEndEntry("CreateLiquidShader");

	auto normalVisualizerShaderHandle = CreateNormalDebugShader();


	auto& liquidShader = GetShader(liquidShaderHandle);
	GLint waveTimeLocation = Uniform::GetLocation(liquidShader.id, "wave_time");

	Random liquidParamRNG;
	LiquidRenderParameters liquidParams = { };
	for (U32 i = 0; i < LIQUID_PARAMETER_COMPONENT_COUNT; i++) {
		liquidParams.amplitudes[i] = liquidParamRNG.Range(0.1f, 0.2f);
		liquidParams.wavelength[i] = liquidParamRNG.Range(3.0f, 9.0f);
		liquidParams.speeds[i] = liquidParamRNG.Range(0.4f, 0.7f);
		liquidParams.directions[i] = Vector2(liquidParamRNG.Range(0.0f, 1.0f), liquidParamRNG.Range(0.0f, 1.0f));
	}
	
	PlatformRun([&](double deltaTime) -> void {
		if (PlatformGetButton(MOUSE_BUTTON_RIGHT)) FPSCameraControlUpdate(&camera);

		terrainStreamer.update(camera.position);

		ProfilerBeginPersistantEntry("Render");
		GFX3D::BeginFrame(camera, &params);
		ProfilerBeginPersistantEntry("Draw Terrain");
		terrain_manager.draw();
		ProfilerEndPersistantEntry("Draw Terrain");


		auto& liquidShader = GetShader(liquidShaderHandle);
		
		ProfilerBeginPersistantEntry("Draw Liquid");
		GFX3D::Begin(liquidShader);
		SetLiquidRenderSettingsUniforms(&liquidParams);
		static float waveTime = 0.0f;
		Uniform::SetFloat(waveTimeLocation, waveTime);
		waveTime += deltaTime;
		GFX3D::DrawMesh(water_mesh, Vector3(terrainCenter.x, 50.0f, terrainCenter.y));
		GFX3D::End();
		ProfilerEndPersistantEntry("Draw Liquid");
		ProfilerEndPersistantEntry("Render");


		//auto& normalShader = GetShader(normalVisualizerShaderHandle);
		//GFX3D::Begin(normalShader);
		//SetLiquidRenderSettingsUniforms(&liquidParams);
		//Uniform::SetFloat(waveTimeLocation, waveTime);
		//GFX3D::DrawMesh(water_mesh, Vector3(terrainCenter.x, 50.0f, terrainCenter.y));
		//GFX3D::End();

		auto& shader = GetShader(terrain_manager.shaderHandle);
		GFX3D::Begin(shader);

		for (auto& entity : g_Entities) {
			auto& model = GetModel(entity.modelHandle);
			GFX3D::DrawModel(model, entity.position);
		}
		GFX3D::End();


		GFX3D::EndFrame();

		ImGui::BeginFrame();
		ShowCameraDebugInfo(camera);
		ShowLightDebugInfo(directionalLight);
		ShowDebugRenderSettings(GetDebugRenderSettings());
		// ShowTerrainProperties(terrain_manager);
		// ShowShaderManifest();
		// ShowMaterialEditor();
		ShowAssetBrowser();
		ShowErrorMessageBox();

		auto profiler = GetGlobalProfiler();
		ShowProfilerGUI(profiler);

		ImGui::EndFrame();

		ProfilerReset();
		FinializeCompletedTasks();
	});

	TerminateTaskManager();
}

int main() {
	RunBasicGLTest();
}
