#include <Core/Platform.h>
#include <Core/Tasks.hpp>

#include <Engine/Editor.hpp>
#include <Engine/Engine.hpp>
#include <Engine/Debug.hpp>
#include <Engine/GFX2D.hpp>
#include <Engine/GFX3D.hpp>
#include <Engine/ECSManager.hpp>
#include <Engine/Terrain.hpp>


#include <Graphics/ModelData.hpp>

#include <Graphics/GLSLProgram.hpp>

#include <Graphics/Mesh.hpp>

#include <Math/Random.hpp>
#include <Math/Procedural3D.hpp>
#include <Math/Vector3.hpp>
#include <Math/Noise.hpp>

#include <Graphics/Render3D.hpp>
#include <Graphics/imgui.h>
#include <Graphics/Liquid.hpp>
#include <Graphics/Lighting.hpp>

#include <Graphics/GLRenderer.hpp>

#include <Utils/utils.hpp>
#include <Utils/Profiler.hpp>
#define IMPLEMENT_GLOBAL_PROFILER
#include <Utils/global_profiler.h>

using namespace Raptor;

#include <Core/config.h>

//static void CreateTestMaterials() {
//	MaterialData data;
//	data.diffuseTextureFilename = ASSET_FILE("materials/dirt0/diffuse.tga");
//	data.normalTextureFilename  = ASSET_FILE("materials/dirt0/normal.tga");
//	SaveMaterialData(data, "dirt.material");
//
//	data.diffuseTextureFilename = ASSET_FILE("materials/grass0/diffuse.tga");
//	data.normalTextureFilename = ASSET_FILE("materials/grass0/normal.tga");
//	SaveMaterialData(data, "grass.material");
//}

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

struct BasicEntity {
	Vector3 position;
	ModelHandle& modelHandle;
};

std::vector<BasicEntity> g_Entities;

template <typename TEntity>
struct EntityManager {
	std::vector<TEntity> entities;
};

struct AtmoshpericFog {
	float density;
	float gradient;
};


void ShowErrorMessageBox() {
	static float timeRemaining = 0;


	if (timeRemaining > 0) {
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 0.9f));
		ImGui::SetNextWindowSize(ImVec2(300.0f, 60.0f));
		ImGui::SetNextWindowPos(ImVec2(8.0f, 8.0f));
		ImGui::Begin("ErrorMessage", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
		ImGui::TextColored(ImVec4(1.0, 0.0, 0.0, 1.0), "[ERROR] This is an error!");
		ImGui::End();
		ImGui::PopStyleColor();
	}

	timeRemaining -= PlatformGetDeltaTime();
}

void RunBasicGLTest() {
	PlatformCreate("Basic GLTest", 1538, 864);
	int width, height;
	PlatformGetSize(&width, &height);


	Engine::Init();
	// auto shaderID = CreateShader("shaders/terrain.vert", "shaders/terrain.frag");

	// CreateTestMaterials();

	ProfilerBeginEntry("Load Models");
	auto rockModelHandle  = LoadModel(ASSET_FILE("models/rock0/Rock1.obj"));
	auto plantModelHandle = LoadModel(ASSET_FILE("models/palm_plant/palm_plant.obj"));
	auto laraModelHandle = LoadModel(ASSET_FILE("models/foliage/tree/Tree.obj"));
	ProfilerEndEntry("Load Models");

	ProfilerBeginEntry("Load Materials");
	auto dirtMaterialHandle	 = LoadMaterial("dirt.material");
	auto grassMaterialHandle = LoadMaterial("grass.material");
	ProfilerEndEntry("Load Materials");

	DirectionalLight directionalLight;
	AddLight(directionalLight);

	EditorWindow editorWindow;
	GFX3D::SetDebugRenderSettings(&editorWindow.renderSettings);

	// UniformDirectionalLight(shaderID, 0, directionalLight);

	// TODO action bar style terrain placer thingy

	static const int WORLD_WIDTH = 9;
	static const int WORLD_HEIGHT = 9;
	static const int WORLD_AREA = WORLD_WIDTH * WORLD_HEIGHT;
	static const float TERRAIN_WIDTH = 128.0f;
	static const float TERRAIN_HEIGHT = 128.0f;
	static const int TERRAIN_RESOLUTION = 128;
	static const int TERRAIN_MATERIAL_COUNT = 2;

	Camera camera((float)width, (float)height);
	camera.position = V3(WORLD_WIDTH * 0.5f * TERRAIN_WIDTH, 80.0f, WORLD_HEIGHT * TERRAIN_HEIGHT * 0.5f);
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
	CreatePlaneMesh(&water_plane, 0.0f, 0.0f, 512.0f, 512.0f, 512, 8);
	InitMesh(&water_mesh, &water_plane);

	ImGui::Init();

	FrameParameters params;
	params.clearColor = Vector3(0.53, 0.77, 0.92);

	Random rng;

	Rectangle terrainRect = terrain_manager.GetBoundingRectangle();
	PointLight pointLights[50];
	for (U32 i = 0; i < 50; i++) {
		pointLights[i].color = Vector3(0.5f) + 
			(Vector3(0.5f * rng.Range(0.0f, 1.0f), 0.5f * rng.Range(0.0f, 1.0f), 0.5f * rng.Range(0.0f, 1.0f)));
		Vector2 xzPair = rng.PointInRectangle(terrainRect); 
		pointLights[i].position = Vector3(xzPair.x, 20, xzPair.y);
		AddLight(&pointLights[i]);
	}

	g_Entities.reserve(512);
	for (U32 i = 0; i < 512; i++) {
		Vector2 xzPair = rng.PointInRectangle(terrainRect);
		g_Entities.emplace_back(BasicEntity { Vector3(xzPair.x, 1.0f, xzPair.y),  plantModelHandle });
	}



	Vector2 terrainCenter = GetCenter(terrainRect);

	//ProfilerBeginEntry("CreateLiquidShader");
	//BeginShaderBuilder("water_shader");
	//AddShaderSourceFile(VERTEX_SHADER, SHADER_FILE("liquid.vert"));
	//AddShaderSourceFile(FRAGMENT_SHADER, SHADER_FILE("liquid.frag"));
	//auto liquidShaderHandle = LoadShaderFromBuilder();
	//ProfilerEndEntry("CreateLiquidShader");

	
	ShaderHandle modelShaderHandle; 
	{
		BeginShaderBuilder("DefaultModel");
		AddShaderSourceFile(VERTEX_SHADER, SHADER_FILE("DeferredGenericMaterial.vert"));
		AddShaderSourceFile(FRAGMENT_SHADER, SHADER_FILE("DeferredGenericMaterial.frag"));
		modelShaderHandle = LoadShaderFromBuilder();
	}


	// auto& liquidShader = GetShader(liquidShaderHandle);
//	GLint waveTimeLocation = Uniform::GetLocation(liquidShader.id, "wave_time");

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
		GFX3D::BeginFrame(&params, &camera);

		ProfilerBeginPersistantEntry("Draw Terrain");
		TerrainManager* terrain = &terrain_manager;
		GFX3D::DrawTerrain(terrain);
		ProfilerEndPersistantEntry("Draw Terrain");

		auto modelShader = GetShader(modelShaderHandle);
		GFX3D::SetMaterialShader(modelShader);
		for (auto& entity : g_Entities) {
			auto& model = GetModel(entity.modelHandle);
			GFX3D::AddModel(model, entity.position);
		}


		//auto& liquidShader = GetShader(liquidShaderHandle);
		//ProfilerBeginPersistantEntry("Draw Liquid");
		//GFX3D::Begin(liquidShader);
		//SetLiquidRenderSettingsUniforms(&liquidParams);
		//static float waveTime = 0.0f;
		//Uniform::SetFloat(waveTimeLocation, waveTime);
		//waveTime += deltaTime;
		//GFX3D::DrawMesh(water_mesh, Vector3(terrainCenter.x, -10.0f, terrainCenter.y));
		//GFX3D::End(liquidShader);
		//ProfilerEndPersistantEntry("Draw Liquid");
		//ProfilerEndPersistantEntry("Render");

		GFX3D::EndFrame();

		ImGui::BeginFrame();

		// ShowTerrainProperties(terrain_manager);
		// ShowShaderManifest();
		// ShowMaterialEditor();


		// ShowErrorMessageBox();

		ShowCameraDebugInfo(camera);
		ShowLightDebugInfo(directionalLight);


		ShowEditorWindow(&editorWindow);

		int keycode;
		while (keycode = PlatformPopKeypress()) {
			ProcessEditorKeypress(&editorWindow, keycode);
		}
		

		ImGui::EndFrame();

		ProfilerReset();
		Engine::Update();


		//glUseProgram(quadShader.id);
		//glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);


	});

	Engine::Terminate();
}

int main() {
	RunBasicGLTest();
}
