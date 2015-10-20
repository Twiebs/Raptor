
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

	void draw();
};



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
	compiler.addSourceFile(VERTEX_SHADER, "shaders/terrain.vert");
	compiler.addSourceFile(FRAGMENT_SHADER, "shaders/terrain.frag");
	compiler.addDefnition("#define MATERIAL_COUNT " + std::to_string(materialCount) + "\n");
	compiler.addDefnition("#define TERRAIN_WIDTH " + std::to_string(terrainWidth) + "\n");
	compiler.addDefnition("#define TERRAIN_LENGTH " + std::to_string(terrainLength) + "\n");
	compiler.addDefnition("#define TERRAIN_CELLS_PER_TEXCOORD " + std::to_string(terrainCellsPerTexcoord) + "\n");

	// compiler.addDefnition("#define TERRAIN_RESOLUTION " + std::to_string(terrainResolution) + "\n");
	shaderID = compiler.compile();
}


inline void BindTexture2DToUnit (GLuint textureID, U32 unitIndex) {
	glActiveTexture(GL_TEXTURE0 + unitIndex);
	glBindTexture(GL_TEXTURE_2D, textureID);
}

void TerrainManager::draw() {
	for (U32 i = 0; i < materialCount; i++) {
		auto& material = GetMaterial(materials[i]);
		BindTexture2DToUnit(material.diffuseMapID, (materialCount * 0) + i);
		BindTexture2DToUnit(alphaMaps[i], (materialCount * 1) + i);
	}

	for (U32 i = 0; i < managerArea; i++) {
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




#define MANAGER_IMPLEMENTATION
void RunBasicGLTest() {
	PlatformCreate("Basic GLTest");
	// auto shaderID = CreateShader("shaders/terrain.vert", "shaders/terrain.frag");

	Camera camera(Vector3(20.0f, 5.0f, 200.0f), 1280.0f, 720.0f);
	camera.farClip = 10000.0f;

	DirectionalLight directionalLight;
	// UniformDirectionalLight(shaderID, 0, directionalLight);


#ifndef MANAGER_IMPLEMENTATION
	static const int WORLD_WIDTH = 1;
	static const int WORLD_HEIGHT = 1;
	static const int WORLD_AREA = WORLD_WIDTH * WORLD_HEIGHT;
	static const float TERRAIN_WIDTH = 128.0f;
	static const float TERRAIN_HEIGHT = 128.0f;
	static const int TERRAIN_GRID_SEGMENTS = 128;
	static const int TERRAIN_MATERIAL_COUNT = 3;

	GLuint alphaMaps[TERRAIN_MATERIAL_COUNT];
	U8* alphaData = (U8*)malloc(TERRAIN_GRID_SEGMENTS * TERRAIN_GRID_SEGMENTS);
	memset(alphaData, 255, TERRAIN_GRID_SEGMENTS * TERRAIN_GRID_SEGMENTS);
	alphaMaps[0] = CreateAlphaMap(TERRAIN_GRID_SEGMENTS, TERRAIN_GRID_SEGMENTS, alphaData);

	OpenSimplexNoise texnoise(0);
	for (U32 n = 0; n < TERRAIN_GRID_SEGMENTS * TERRAIN_GRID_SEGMENTS; n++) {
		auto x = n % TERRAIN_GRID_SEGMENTS;
		auto z = n / TERRAIN_GRID_SEGMENTS;
		auto value = texnoise.FBM(x, z, 6, 0.001f, 0.05f);
		alphaData[n] = (U8)(value * 255);
	}
	alphaMaps[1] = CreateAlphaMap(TERRAIN_GRID_SEGMENTS, TERRAIN_GRID_SEGMENTS, alphaData);

	OpenSimplexNoise noise0(34534);
	for (U32 n = 0; n < TERRAIN_GRID_SEGMENTS * TERRAIN_GRID_SEGMENTS; n++) {
		auto x = n % TERRAIN_GRID_SEGMENTS;
		auto z = n / TERRAIN_GRID_SEGMENTS;
		auto value = noise0.FBM(x, z, 6, 0.1f, 0.5f);
		alphaData[n] = (U8)(value * 255);
	}
	alphaMaps[2] = CreateAlphaMap(TERRAIN_GRID_SEGMENTS, TERRAIN_GRID_SEGMENTS, alphaData);


	free(alphaData);

	MeshData terrainDatas[WORLD_AREA];
	Mesh terrainMeshes[WORLD_AREA];
	for (U32 i = 0; i < WORLD_AREA; i++) {
		auto& terrainMesh = terrainMeshes[i];
		auto& terrainData = terrainDatas[i];
		auto terrainX = (i % WORLD_WIDTH) * TERRAIN_WIDTH;
		auto terrainY = (i / WORLD_HEIGHT) * TERRAIN_HEIGHT;
		CreatePlaneMesh(&terrainData, terrainX, terrainY, TERRAIN_WIDTH, TERRAIN_HEIGHT, TERRAIN_GRID_SEGMENTS, 4, [](float x, float y) {
			OpenSimplexNoise noise(0);
			return noise.FBM(x, y, 6, 0.005f, 0.5f) * 80;
		});

		InitMesh(&terrainMesh, &terrainData);
		terrainMesh.materialID = LoadMaterial("assets/materials/dirt0/diffuse.tga", "assets/materials/toon_stone0/diffuse.tga", "assets/materials/smooth_stone/diffuse.tga");
	}

#else

//	static const int WORLD_WIDTH = 1;
//	static const int WORLD_HEIGHT = 1;
//	static const int WORLD_AREA = WORLD_WIDTH * WORLD_HEIGHT;
//	static const float TERRAIN_WIDTH = 128.0f;
//	static const float TERRAIN_HEIGHT = 128.0f;
//	static const int TERRAIN_GRID_SEGMENTS = 128;
//
//	U8* alphaData = (U8*)malloc(TERRAIN_GRID_SEGMENTS * TERRAIN_GRID_SEGMENTS);
//	memset(alphaData, 255, TERRAIN_GRID_SEGMENTS * TERRAIN_GRID_SEGMENTS);
//
//	TerrainManager terrain_manager(3, WORLD_WIDTH, WORLD_HEIGHT, TERRAIN_GRID_SEGMENTS);
//	terrain_manager.alphaMaps[0] = CreateAlphaMap(TERRAIN_GRID_SEGMENTS, TERRAIN_GRID_SEGMENTS, alphaData);
//	terrain_manager.materials[0] = LoadMaterial("assets/materials/dirt0/diffuse.tga", "assets/materials/toon_stone0/diffuse.tga", "assets/materials/smooth_stone/diffuse.tga");
//	terrain_manager.materials[1] = LoadMaterial("assets/materials/dirt0/diffuse.tga", "assets/materials/toon_stone0/diffuse.tga", "assets/materials/smooth_stone/diffuse.tga");
//	terrain_manager.materials[2] = LoadMaterial("assets/materials/dirt0/diffuse.tga", "assets/materials/toon_stone0/diffuse.tga", "assets/materials/smooth_stone/diffuse.tga");
//
//	OpenSimplexNoise texnoise(0);
//	for (U32 n = 0; n < TERRAIN_GRID_SEGMENTS * TERRAIN_GRID_SEGMENTS; n++) {
//		auto x = n % TERRAIN_GRID_SEGMENTS;
//		auto z = n / TERRAIN_GRID_SEGMENTS;
//		auto value = texnoise.FBM(x, z, 6, 0.001f, 0.05f);
//		alphaData[n] = (U8)(value * 255);
//	}
//	terrain_manager.alphaMaps[1] = CreateAlphaMap(TERRAIN_GRID_SEGMENTS, TERRAIN_GRID_SEGMENTS, alphaData);
//
//	OpenSimplexNoise noise0(34534);
//	for (U32 n = 0; n < TERRAIN_GRID_SEGMENTS * TERRAIN_GRID_SEGMENTS; n++) {
//		auto x = n % TERRAIN_GRID_SEGMENTS;
//		auto z = n / TERRAIN_GRID_SEGMENTS;
//		auto value = noise0.FBM(x, z, 6, 0.1f, 0.5f);
//		alphaData[n] = (U8)(value * 255);
//	}
//	terrain_manager.alphaMaps[2] = CreateAlphaMap(TERRAIN_GRID_SEGMENTS, TERRAIN_GRID_SEGMENTS, alphaData);
//
//
//	free(alphaData);
//
//	MeshData terrainDatas[WORLD_AREA];
//	for (U32 i = 0; i < WORLD_AREA; i++) {
//		auto& terrainMesh = terrain_manager.terrainMeshes[i];
//		auto& terrainData = terrainDatas[i];
//		auto terrainX = (i % WORLD_WIDTH) * TERRAIN_WIDTH;
//		auto terrainY = (i / WORLD_HEIGHT) * TERRAIN_HEIGHT;
//		CreatePlaneMesh(&terrainData, terrainX, terrainY, TERRAIN_WIDTH, TERRAIN_HEIGHT, TERRAIN_GRID_SEGMENTS, 4, [](float x, float y) {
//			OpenSimplexNoise noise(0);
//			return noise.FBM(x, y, 6, 0.005f, 0.5f) * 80;
//		});
//
//		InitMesh(&terrainMesh, &terrainData);
//	}

	// TODO action bar style terrain placer thingy

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
	auto MaterialMaskGenerationProc = [&materialNoise, &terrain_manager](float x, float y, U32 materialIndex) -> U8 {
		if (materialIndex % terrain_manager.materialCount == 0) return 255;
		auto value = materialNoise.FBM(x, y, 6, 0.01f, 0.5f);
		value = (value + 1.0f) / 2.0f;
		return (U8)(value * 255);
	};


	for (U32 i = 0; i < terrain_manager.terrainCount; i++) {
		for (U32 n = 0; n < terrain_manager.materialCount; n++) {
			U32 terrainXIndex = i % terrain_manager.managerMaxWidth;
			U32 terrainZIndex = i / terrain_manager.managerMaxWidth;
			for (U32 z = 0; z < terrain_manager.terrainResolution; z++) {
				for (U32 x = 0; x < terrain_manager.terrainResolution; x++) {
					U32 alphaIndex = (z * terrain_manager.terrainResolution) + x;
					alphaData[alphaIndex] = MaterialMaskGenerationProc(x + (terrainXIndex * terrain_manager.terrainWidth), z + (terrainZIndex * terrain_manager.terrainLength), n);
				}
			}

			terrain_manager.alphaMaps[(i * terrain_manager.materialCount) + n] = CreateAlphaMap(terrain_manager.terrainResolution, terrain_manager.terrainResolution, alphaData);
		}
	}


//
//	OpenSimplexNoise texnoise(0);
//	for (U32 n = 0; n < TERRAIN_RESOLUTION * TERRAIN_RESOLUTION; n++) {
//		auto x = n % TERRAIN_RESOLUTION;
//		auto z = n / TERRAIN_RESOLUTION;
//		auto value = texnoise.FBM(x, z, 6, 0.001f, 0.05f);
//		alphaData[n] = (U8)(value * 255);
//	}
//	terrain_manager.alphaMaps[1] = CreateAlphaMap(TERRAIN_RESOLUTION, TERRAIN_RESOLUTION, alphaData);
//
//	OpenSimplexNoise noise0(34534);
//	for (U32 n = 0; n < TERRAIN_RESOLUTION * TERRAIN_RESOLUTION; n++) {
//		auto x = n % TERRAIN_RESOLUTION;
//		auto z = n / TERRAIN_RESOLUTION;
//		auto value = noise0.FBM(x, z, 6, 0.1f, 0.5f);
//		alphaData[n] = (U8)(value * 255);
//	}
//	terrain_manager.alphaMaps[2] = CreateAlphaMap(TERRAIN_RESOLUTION, TERRAIN_RESOLUTION, alphaData);
//

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

	terrain_manager.materials[0] = LoadMaterial("assets/materials/dirt0/diffuse.tga", "assets/materials/toon_stone0/diffuse.tga", "assets/materials/smooth_stone/diffuse.tga");
	terrain_manager.materials[1] = LoadMaterial("assets/materials/toon_stone0/diffuse.tga", "assets/materials/toon_stone0/diffuse.tga", "assets/materials/smooth_stone/diffuse.tga");
	//terrain_manager.materials[2] = LoadMaterial("assets/materials/smooth_stone/diffuse.tga", "assets/materials/toon_stone0/diffuse.tga", "assets/materials/smooth_stone/diffuse.tga");

#endif


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


		// BeginWireframe();
#ifdef MANAGER_IMPLEMENTATION
		terrain_manager.draw();
#else
		for (U32 i = 0; i < WORLD_AREA; i++) {
			auto& terrainMesh = terrainMeshes[i];
//			glActiveTexture(GL_TEXTURE0);
//			glBindTexture(GL_TEXTURE_2D, materials[0].diffuseMapID);
			auto& material = GetMaterial(terrainMesh.materialID);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, material.diffuseMapID);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, material.specularMapID);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, material.normalMapID);

			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, alphaMaps[0]);
			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, alphaMaps[1]);
			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_2D, alphaMaps[2]);


			glBindVertexArray(terrainMesh.vertexArrayID);
			glDrawElements(GL_TRIANGLES, terrainMesh.indexCount, GL_UNSIGNED_INT, 0);
		}
#endif

		// EndWireframe();

		ImGui::BeginFrame();
		ShowCameraDebug(camera);
		ShowLightParamaters(directionalLight);
		ImGui::EndFrame();
	});
}

int main() {
	RunBasicGLTest();
}
