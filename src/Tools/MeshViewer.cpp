
#include <Core/Common.hpp>
#include <Core/Application.hpp>
#include <World/World.hpp>
#include <World/Editor.hpp>

#include <Math/Geometry2D.hpp>
#include <Math/Geometry3D.hpp>
#include <Math/Matrix4.hpp>
#include <Math/Random.hpp>
#include <Math/Noise.hpp>
#include <Math/Vector3.hpp>
#include <Math/Vector2.hpp>

#include <Graphics/GLSLProgram.hpp>
#include <Graphics/Texture.hpp>
#include <Graphics/Render3D.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Graphics/GUI.hpp>

#include <SDL2/SDL.h>
#undef main

#define SHADOWMAP_WIDTH 2048
#define SHADOWMAP_HEIGHT 2048

using namespace Raptor;

namespace Profiler {
	struct ProfileEntry {
		std::string label;
		U64 ms;
		U64 cycles;
		float history[64];
		U32 historyIndex = 0;
	};

	std::vector<U32> history;
	std::vector<ProfileEntry> entries;

	void Begin(const std::string& label) {
		for (auto i = 0; i < entries.size(); i++) {
			auto& entry = entries[i];
			if (entry.label == label) {
				entry.cycles = __rdtsc();
				entry.ms = SDL_GetPerformanceCounter();
				history.push_back(i);
				return;
			}
		}

		history.push_back(entries.size());
		ProfileEntry entry;
		entry.label = label;
		entry.cycles = __rdtsc();
		entry.ms = SDL_GetPerformanceCounter();
		entries.emplace_back(entry);
	}

	void End() {
		auto& entry = entries[history[history.size() - 1]];
		history.pop_back();
		entry.cycles = __rdtsc() - entry.cycles;
		entry.ms = SDL_GetPerformanceCounter() - entry.ms;
		entry.history[entry.historyIndex++] = float(entry.ms / SDL_GetPerformanceFrequency());
		if (entry.historyIndex >= 64) entry.historyIndex = 0;
	}

	void ShowWindow() {
		ImGui::Begin("Profiler");
		for (auto& entry : entries) {
			ImGui::Text(entry.label.c_str());
			ImGui::Text("CPU Cycles: %d", entry.cycles);
			ImGui::PlotLines(entry.label.c_str(), entry.history, 64, entry.historyIndex, "ms");
			ImGui::Separator();
		}
		ImGui::End();
	}
}


// NOTE the extra fielname field is for shaderReloading
//I dont really care about this for now.
//This has the potental to represent an asset file inside a pack
//There fore in the future it is going to need to required an assetTypeID;
//There could also be a userdata field inorder to allow custom types!
struct Asset {
	std::string filename;
	std::string filenameExtra;
	Asset();
	Asset(const std::string& filename);
	Asset(const std::string& filename, const std::string& filenameExtra);
};

Asset::Asset() {

}

Asset::Asset(const std::string& filename) {
	this->filename = filename;
}

Asset::Asset(const std::string& filename, const std::string& filenameExtra) {
	this->filename = filename;
	this->filenameExtra = filenameExtra;
}

void CreateVertexBuffersForMesh(const MeshData& mesh, VertexBufferGroup& group) {
	glGenVertexArrays(1, &group.vertexArrayID);
	glBindVertexArray(group.vertexArrayID);

	glGenBuffers(1, &group.vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, group.vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, mesh.vertexCount * sizeof(Vertex3D), mesh.vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &group.elementBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, group.elementBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indexCount * sizeof(U32), mesh.indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)offsetof(Vertex3D, position));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)offsetof(Vertex3D, normal));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)offsetof(Vertex3D, texCoord));
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)offsetof(Vertex3D, tangent));
	glBindVertexArray(0);
}

struct AssetTable {
	std::vector<Asset> modelAssets;
	std::vector<DebugModelData> models;

	std::vector<Asset> shaderAssets;
	std::vector<GLSLProgram> shaderPrograms;
};

//@GLOBALS
global_variable AssetTable global_assetTable;

U32 LoadShader(const std::string& vertexFilename, const std::string& fragmentFilename) {
	global_assetTable.shaderPrograms.emplace_back(vertexFilename, fragmentFilename);
	global_assetTable.shaderAssets.emplace_back(vertexFilename, fragmentFilename);
	return global_assetTable.shaderPrograms.size();
}

U32 LoadShader(const std::string& vertexFilename, const std::string& fragmentFilename, const std::string& geometryFilename) {
	global_assetTable.shaderPrograms.emplace_back(vertexFilename, fragmentFilename, geometryFilename);
	global_assetTable.shaderAssets.emplace_back(vertexFilename, fragmentFilename);
	return global_assetTable.shaderPrograms.size();
}

void ShowAssetExplorer(AssetTable& table) {
	ImGui::Begin("Asset Explorer");
	if (ImGui::TreeNode("Assets")) {
		if (ImGui::TreeNode("Models")) {
			for (auto i = 0; i < table.modelAssets.size(); i++) {
				auto& asset = table.modelAssets[i];
				ImGui::Text(asset.filename.c_str());
				ImGui::SameLine();
				ImGui::PushID(i);
				if (ImGui::SmallButton("Reload##%d")) {
					auto& model = table.models[i];
					model.materials.clear();
					model.meshes.clear();
					model.meshMaterialIndex.clear();
					model.meshVertexBuffers.clear();
					model.LoadFromFile(asset.filename);
				}
				ImGui::PopID();
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Shaders")) {
			for (auto i = 0; i < table.shaderPrograms.size(); i++) {
				auto& asset = table.shaderAssets[i];
				ImGui::Text(asset.filename.c_str());
				ImGui::SameLine();
				ImGui::PushID(i);
				if (ImGui::SmallButton("Reload")) {
					auto& shader = table.shaderPrograms[i];
					shader.Dispose();
					shader.Load(asset.filename, asset.filenameExtra);
				}
				ImGui::PopID();
			}
			ImGui::TreePop();
		} 


		ImGui::TreePop();
	}
	ImGui::End();
}

class DebugMesh {
public:
	GLuint vertexArrayID;
	Vertex3D* vertices;
	U32* indices;
	U32 vertexCount;
	U32 indexCount;

private:
	GLuint vertexBufferID;
	GLuint elementBufferID;
	U8* memory;

public:
	DebugMesh();
	DebugMesh(U32 vertexCount, U32 indexCount);
	~DebugMesh();

	void Init(U32 vertexCount, U32 indexCount);
	void InitGPU();
};

DebugMesh::DebugMesh() {
	vertexCount = 0;
}

DebugMesh::DebugMesh(U32 vertexCount, U32 indexCount) {
	Init(vertexCount, indexCount);
}

void DebugMesh::Init(U32 vertexCount, U32 indexCount) {
	this->vertexCount = vertexCount;
	this->indexCount = indexCount;
	auto verticesMemorySize = sizeof(Vertex3D) * vertexCount;
	auto indiciesMemorySize = sizeof(U32) * indexCount;
	memory = new U8[verticesMemorySize + indiciesMemorySize];
	vertices = (Vertex3D*)memory;
	indices = (U32*)(memory + verticesMemorySize);
}

void CalculateNormals(DebugMesh& mesh) {
	for (uint32 i = 0; i < mesh.vertexCount; i++) {
		mesh.vertices[i].normal = Vector3(0.0f, 0.0f, 0.0f);
	}
	for (uint32 i = 0; i < mesh.indexCount; i += 3) {
		uint32 index0 = mesh.indices[i + 0];
		uint32 index1 = mesh.indices[i + 1];
		uint32 index2 = mesh.indices[i + 2];
		Vector3 vert1 = mesh.vertices[index1].position - mesh.vertices[index0].position;
		Vector3 vert2 = mesh.vertices[index2].position - mesh.vertices[index0].position;

		Vector3 normal = vert1.Cross(vert2);
		normal.Normalize();
		mesh.vertices[index0].normal += normal;
		mesh.vertices[index1].normal += normal;
		mesh.vertices[index2].normal += normal;
	}
	for (uint32 i = 0; i < mesh.vertexCount; i++) {
		mesh.vertices[i].normal.Normalize();
	}
}

void DebugMesh::InitGPU() {
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	glGenBuffers(1, &vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(Vertex3D), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &elementBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(U32), indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)offsetof(Vertex3D, position));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)offsetof(Vertex3D, normal));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)offsetof(Vertex3D, texCoord));
	glBindVertexArray(0);
}

DebugMesh::~DebugMesh() {
	if (memory != nullptr) {
		delete[] memory;
	}
	if (vertexArrayID != 0) {
		glDeleteVertexArrays(1, &vertexArrayID);
		glDeleteBuffers(1, &vertexBufferID);
		glDeleteBuffers(1, &elementBufferID);
	}
}

void Draw(const DebugMesh& mesh) {
	glBindVertexArray(mesh.vertexArrayID);
	glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, 0);
}


inline U32 IndexOf(U32 x, U32 y, size_t stride) {
	auto result = (y * stride) + x;
	return result;
}

void MakeQuad(DebugMesh& mesh, float width, float height, U32 rows, U32 cols) {
	assert(mesh.vertices == nullptr && mesh.vertexCount == 0); //Mesh must not be initalized
	auto vertexCount = (rows * 2) * (cols * 2);
	auto indexCount = (rows * cols) * 6;
	mesh.Init(vertexCount, indexCount);

	auto vertexIndex = 0;
	auto indiceIndex = 0;
	for (auto y = 0; y < cols; y++) {
		for (auto x = 0; x < rows ; x++) {
			auto vertex0 = &mesh.vertices[vertexIndex++];
			vertex0->position = Vector3((float)(x) * (width / (float)rows), 0.0f, (float)(y) * (height / (float)cols));
			vertex0->normal = Vector3(0.0f, 1.0f, 0.0f);
			vertex0->texCoord = Vector2(0.0f, 0.0f);
			
			auto vertex1 = &mesh.vertices[vertexIndex++];
			vertex1->position = Vector3((float)(x + 1)* (width / (float)rows), 0.0f, (float)(y)* (height / (float)cols));
			vertex1->normal = Vector3(0.0f, 1.0f, 0.0f);
			vertex1->texCoord = Vector2(1.0f, 0.0f);

			auto vertex2 = &mesh.vertices[vertexIndex++];
			vertex2->position = Vector3((float)(x + 1)* (width / (float)rows), 0.0f, (float)(y + 1)* (height / (float)cols));
			vertex2->normal = Vector3(0.0f, 1.0f, 0.0f);
			vertex2->texCoord = Vector2(1.0f, 1.0f);

			auto vertex3 = &mesh.vertices[vertexIndex++];
			vertex3->position = Vector3((float)(x)* (width / (float)rows), 0.0f, (float)(y + 1)* (height / (float)cols));
			vertex3->normal = Vector3(0.0f, 1.0f, 0.0f);
			vertex3->texCoord = Vector2(0.0f, 1.0f);

			//Tangent Vector Calcuation
			auto edge0 = vertex1->position - vertex0->position;
			auto edge1 = vertex2->position - vertex0->position;
			auto deltaUV0 = vertex1->texCoord - vertex0->texCoord;
			auto deltaUV1 = vertex2->texCoord - vertex0->texCoord;

			Vector3 tangent;
			auto f = 1.0f / ((deltaUV0.x * deltaUV1.y) - (deltaUV1.x * deltaUV0.y));
			tangent.x = f * ((deltaUV1.y * edge0.x) - (deltaUV0.y * edge1.x));
			tangent.y = f * ((deltaUV1.y * edge0.y) - (deltaUV0.y * edge1.y));
			tangent.y = f * ((deltaUV1.y * edge0.z) - (deltaUV0.y * edge1.z));
			tangent.Normalize();
			//NOTE This doesnt include the second triangle of the plane
			//This is an oversimplification of the problem!
			//TODO Fix Tangent Calcuation
			vertex0->tangent = tangent;
			vertex1->tangent = tangent;
			vertex2->tangent = tangent;
			vertex2->tangent = tangent;

			mesh.indices[indiceIndex++] = vertexIndex - 4;
			mesh.indices[indiceIndex++] = vertexIndex - 1;
			mesh.indices[indiceIndex++] = vertexIndex - 2;
			mesh.indices[indiceIndex++] = vertexIndex - 4;
			mesh.indices[indiceIndex++] = vertexIndex - 2;
			mesh.indices[indiceIndex++] = vertexIndex - 3;
		}
	}
	mesh.InitGPU();
}

void MakeTerrain(DebugMesh& mesh, float width, float height, U32 rows, U32 cols) {
	assert(mesh.vertices == nullptr && mesh.vertexCount == 0); //Mesh must not be initalized
	auto vertexCount = (rows + 1) * (cols + 1);
	auto indexCount = (rows * cols) * 6;
	mesh.Init(vertexCount, indexCount);
	
	auto borderSize = rows * 0.1f;
	Rectangle bounds{ borderSize, borderSize, rows - (borderSize * 2), cols - (borderSize * 2) };
	OpenSimplexNoise noise(0);
	for (auto y = 0; y < cols + 1; y++) {
		for (auto x = 0; x < rows + 1; x++) {
			auto index = IndexOf(x, y, rows + 1);
			auto& vertex = mesh.vertices[index];
			auto value = noise.FBM(x, y, 5, 0.09f, 0.5f);
			if (!IsPointInRect(x, y, bounds)) {
				value = (value + 1.0f) * 5.0f;
			}

			vertex.position = Vector3((float)x * (width / (float)rows), value * 6, (float)y * (height / (float)cols));
			//vertex.normal = Vector3(0.0f, 1.0f, 0.0f);
			//vertex.texCoord = Vector2((float)x / (float)rows, (float)y / (float)cols);
			auto u = x % 2;
			auto v = y % 2;
			vertex.texCoord = Vector2(u, v);
		}
	}

	auto index = 0;
	for (auto y = 0; y < cols; y++) {
		for (auto x = 0; x < rows; x++) {
			auto index0 = IndexOf(x, y, rows + 1);
			auto index1 = IndexOf(x + 1, y, rows + 1);
			auto index2 = IndexOf(x + 1, y + 1, rows + 1);
			auto index3 = IndexOf(x, y + 1, rows + 1);
			mesh.indices[index + 0] = index0;
			mesh.indices[index + 1] = index1;
			mesh.indices[index + 2] = index2;
			mesh.indices[index + 3] = index2;
			mesh.indices[index + 4] = index3;
			mesh.indices[index + 5] = index0;
			index += 6;
		}
	}

	CalculateNormals(mesh);

	mesh.InitGPU();
}

void LoadModel(const std::string& filename) { 
	global_assetTable.models.emplace_back(DebugModelData());
	auto& model = global_assetTable.models[global_assetTable.models.size() - 1];
	model.LoadFromFile(filename);
	global_assetTable.modelAssets.emplace_back(filename);
}

U32 LoadShaderProgram(const std::string& vertexFilename, const std::string& fragmentFilename) {
	global_assetTable.shaderPrograms.emplace_back(GLSLProgram(vertexFilename, fragmentFilename));
	global_assetTable.shaderAssets.emplace_back(vertexFilename, fragmentFilename);
	return global_assetTable.shaderPrograms.size() - 1;
}

GLSLProgram& GetShader(U32 assetID) {
	return global_assetTable.shaderPrograms[assetID - 1];
}

GLSLProgram& GetAndUseShader(U32 assetID) {
	auto& result = global_assetTable.shaderPrograms[assetID - 1];
	result.Use();
	return result;
}

struct Terrain {
	DebugMesh mesh;
	Material material;
};

Terrain G_Terrain;

void CreateTerrain(Terrain& terrain) {
	terrain.material.diffuseMapID = CreateTextureFromFile("materials/stone_tile/diffuse.tga");
	terrain.material.specularMapID = CreateTextureFromFile("materials/stone_tile/specular.tga");
	terrain.material.normalMapID = CreateTextureFromFile("materials/stone_tile/normal.tga");
	//terrain.material.diffuseMapID = CreateTextureFromFile("materials/green_tile/diffuse.tga");
	//terrain.material.specularMapID = CreateTextureFromFile("materials/green_tile/specular.tga");
	//terrain.material.normalMapID = CreateTextureFromFile("materials/green_tile/normal.tga");
	MakeQuad(terrain.mesh, 256.0f, 256.0f, 128, 128);
}

void Draw(Terrain& terrain) {
	BindMaterial(terrain.material);
	Draw(terrain.mesh);
};

void DebugRenderWorldLights(World& world, GLuint shader, Camera& camera) {
	glUseProgram(shader);
	static GLint mvpLoc = GetUniformLocation(shader, "mvp");
	static GLint lightColorLoc = GetUniformLocation(shader, "lightColor");

	Matrix4 modelMatrix;
	for (auto i = 0; i < world.pointLightCount; i++) {
		auto& light = (PointLight&)world.components[COMPONENT_POINTLIGHT][i];
		modelMatrix = Matrix4::Translate(light.position);
		Matrix4 mvp = camera.projection * camera.view * modelMatrix;
		glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &mvp[0][0]);
		glUniform3fv(lightColorLoc, 1, &light.color.x);
		DrawDebugCube();
	}
}

//Load Models, Populate with entities, CreateTerrain
void PopulateTestWorld(World& world) {
	auto createScatterMap = [&world](float x, float y, float width, float height, U32 rows, U32 cols, Transform3D* transforms) {
		Random rng(0);
		OpenSimplexNoise noise(0);

		auto cellWidth = width / rows;
		auto cellHeight = height / cols;
		auto scatter = 0.4f;

		U32 count = 0;
		auto modelIDs = (U32*)world.components[COMPONENT_MODELID];
		for (auto iy = 0; iy < rows; iy++) {
			for (auto ix = 0; ix < cols; ix++) {
				auto xpos = x + (cellWidth * ix) + (rng.Range(-scatter, scatter) * cellWidth);
				auto zpos = y + (cellHeight * iy) + (rng.Range(-scatter, scatter)*cellHeight);
				transforms[count].translation = Vector3(xpos, 0.0f, zpos);
				transforms[count].rotation = Vector3(0.0f, 0.0f, 0.0f);
				transforms[count].scale = Vector3(1.0f, 1.0f, 1.0f);
				modelIDs[count] = rng.Range(0, 1);
				count++;
				CreateEntity(world);
			}
		}
	};

	createScatterMap(0.0f, 0.0f, 256.0f, 256.0f, 8, 8, (Transform3D*)world.components[COMPONENT_TRANSFORM]);
}

struct GPUConfig {
	bool useNormalMaping;
	bool castShadows;
	int drawMode = 0;
};

global_variable GLuint global_debugLightShader;

global_variable DepthShader global_depthShader;
global_variable FowardShader global_fowardShader;
global_variable DeferredShader global_deferredShader;
global_variable GPUConfig global_GPUConfig;
global_variable GLuint global_uniformBufferID;

global_variable World global_world;
global_variable Editor global_editor;

void AddRandomLights(World& world, U32 count) {
	Random rng;

	PointLight* pointLights = (PointLight*)world.components[COMPONENT_POINTLIGHT];
	for (auto i = 0; i < count; i++) {
		PointLight& light = pointLights[i];
		light.position = Vector3(rng.Range(0.0f, 256.0f), rng.Range(1.0f, 3.0f), rng.Range(0.0f, 256.0f));
		light.color = Vector3(rng.Range(0.25f, 1.0f), rng.Range(0.25f, 1.0f), rng.Range(0.25f, 1.0f));
		light.linear = 0.027f;
		light.quadratic = 0.0028f;
		GLfloat lightMax = std::fmaxf(std::fmaxf(light.color.x, light.color.y), light.color.z);
		light.radius = (-light.linear + std::sqrtf(light.linear * light.linear - 4 * light.quadratic * (1.0 - (256.0 / 5.0) * lightMax))) / (2 * light.quadratic);
	}
	world.pointLightCount = count;
}

void DrawWorld(World& world, GLuint shader) {
	Matrix4 modelMatrix;
	modelMatrix = Matrix4::Identity();
	glUniformMatrix4fv(MODEL_LOCATION, 1, GL_FALSE, &modelMatrix[0][0]);
	Draw(G_Terrain);

	Transform3D* transforms = (Transform3D*)world.components[COMPONENT_TRANSFORM];
	U32* modelIDs = (U32*)world.components[COMPONENT_MODELID];
	for (auto i = 0; i < world.entityCount; i++) {
		modelMatrix = TransformToMatrix(transforms[i]);
		glUniformMatrix4fv(MODEL_LOCATION, 1, GL_FALSE, &modelMatrix[0][0]);
		Draw(global_assetTable.models[modelIDs[i]]);
	}
}

void RenderWorld(World& world, Raptor::DeferredShader& shader, Raptor::Camera& camera) {
	BeginDeferredShadingGeometryPass(shader, camera);
	DrawWorld(world, shader.geometeryPassProgram);
	Raptor::EndDeferredShadingGeometeryPass();

	Raptor::BeginDeferredShadingLightingPass(shader, camera);
	glUniform1i(GetUniformLocation(global_deferredShader.lightingPassProgram, "draw_mode"), global_GPUConfig.drawMode);
	PointLight* pointLights = (PointLight*)world.components[COMPONENT_POINTLIGHT];
	for (auto i = 0; i < world.pointLightCount; i++) {
		Raptor::PushLight(pointLights[i], i, shader.lightingPassProgram);
	}
	Raptor::EndDeferredShadingLightingPass(shader);
	DebugRenderWorldLights(world, global_debugLightShader, camera);
}

void RenderWorld(World& world, FowardShader& shader, DepthShader& depthShader, Camera& camera) {
	BeginDepthShadingPass(depthShader);

	PointLight* pointLights = (PointLight*)world.components[COMPONENT_POINTLIGHT];
	for (auto i = 0; i < world.pointLightCount; i++) {
		DepthShadingPushLight(pointLights[i], depthShader);
	}
	DrawWorld(world, depthShader.program);
	EndDepthShadingPass();

	BeginFowardShadingPass(shader, depthShader, camera);
	DrawWorld(world, shader.program);

	for (auto i = 0; i < world.pointLightCount; i++) {
		PushLight(pointLights[i], i, shader.program);
	}
	EndFowardShadingPass();

	DebugRenderWorldLights(world, global_debugLightShader, camera);
}

void CreateConfigUniformBuffer(GLuint* uniformBufferID) {
	glGenBuffers(1, uniformBufferID);
	glBindBuffer(GL_UNIFORM_BUFFER, *uniformBufferID);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(GPUConfig), &global_GPUConfig, GL_DYNAMIC_DRAW);

	//Move this later
	GLuint blockIndex = glGetUniformBlockIndex(global_fowardShader.program, "config");
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, *uniformBufferID);
	glUniformBlockBinding(global_fowardShader.program, blockIndex, 0);
	blockIndex = glGetUniformBlockIndex(global_fowardShader.program, "config");
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, *uniformBufferID);
	glUniformBlockBinding(global_fowardShader.program, blockIndex, 1);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UpdateConfigUniforms(GLuint uniformBufferID) {
	glBindBuffer(GL_UNIFORM_BUFFER, uniformBufferID);
	GLvoid* buffer = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
	memcpy(buffer, &global_GPUConfig, sizeof(GPUConfig));;
	glUnmapBuffer(GL_UNIFORM_BUFFER);
}

void MainLoop(Application* app) {
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	app->SetCursorHidden(false);
	if (app->IsButtonDown(MOUSE_BUTTON_RIGHT)) {
		app->SetCursorHidden(true);
		FPSCameraControlUpdate(app, global_editor.camera);
	}
	UpdateCamera(global_editor.camera);

	glViewport(0, 0, app->GetWidth(), app->GetHeight());
	static bool useFowardRendering = false;
	if (useFowardRendering) RenderWorld(global_world, global_fowardShader, global_depthShader, global_editor.camera);
	else RenderWorld(global_world, global_deferredShader, global_editor.camera);

	static GUIContext guiContext{ 0 };
	if (guiContext.vertexArrayID == 0) GUIContextInit(&guiContext, app->GetWidth(), app->GetHeight());
	GUIBeginFrame(&guiContext, app);

	ImGui::Begin("Render Settings");
	ImGui::Checkbox("Use Foward Shading", &useFowardRendering);
	if (ImGui::Checkbox("Use Normal Maps", &global_GPUConfig.useNormalMaping)) UpdateConfigUniforms(global_uniformBufferID);
	if (ImGui::Checkbox("CastShadows", &global_GPUConfig.castShadows)) UpdateConfigUniforms(global_uniformBufferID);
	
	ImGui::RadioButton("All", &global_GPUConfig.drawMode, 0);
	ImGui::RadioButton("Position", &global_GPUConfig.drawMode, 1);
	ImGui::RadioButton("Normal", &global_GPUConfig.drawMode, 2);
	ImGui::RadioButton("Diffuse", &global_GPUConfig.drawMode, 3);
	ImGui::RadioButton("Specular", &global_GPUConfig.drawMode, 4);

	ImGui::End();

	ShowWorldEditor(global_world, global_editor);
	ShowEntityInspector(global_world, global_editor);

	ImGui::Text("Camera Position [%f, %f, %f]", global_editor.camera.position.x, global_editor.camera.position.y, global_editor.camera.position.z);
	ImGui::Text("Yaw: %f", global_editor.camera.yaw);
	ImGui::Text("Pitch: %f", global_editor.camera.pitch);
	GUIEndFrame();
}

#if 1
int main () {
    Application app("Raptor Mesh Viewer", 1280, 720, false);
	glEnable(GL_DEPTH_TEST);

	CreateConfigUniformBuffer(&global_uniformBufferID);

	global_assetTable.shaderPrograms.resize(7);
	global_editor.camera.position = Vector3(128.0f, 2.0f, 128.0f);
	global_editor.camera.viewportWidth = app.GetWidth();
	global_editor.camera.viewportHeight = app.GetHeight();
	global_editor.camera.nearClip = 0.1f;
	global_editor.camera.farClip = 1000.0f;

	CreateTerrain(G_Terrain);
#if 0
	CreateTestWorld(global_world);
	PopulateTestWorld(global_world);
	SaveWorld(global_world, "test.world");
#endif

#if 1
	LoadWorld(global_world, "test.world");
	LoadEditor(global_editor, "test.editor");
	//for (auto i = 0; i < 64; i++) {
	//	global_editor.entityNames[i] = "entity" + std::to_string(i);
	//}

#endif

	AddRandomLights(global_world, 64);
	InitDepthShader(global_depthShader, 2048, 2048);
	InitFowardShader(global_fowardShader);
	InitDeferredShader(global_deferredShader, app.GetWidth(), app.GetHeight());
	glUseProgram(global_deferredShader.lightingPassProgram);
	glUniform1i(GetUniformLocation(global_deferredShader.lightingPassProgram, "pointLightCount"), global_world.pointLightCount);

	auto& light = (PointLight&)global_world.components[COMPONENT_POINTLIGHT][0];
	light.position = Vector3(10.0f, 10.0f, 20.0f);
	light.color = Vector3(1.0f, 1.0f, 1.0f);
	light.linear = 0.027f;
	light.quadratic = 0.0028f;
	GLfloat lightMax = std::fmaxf(std::fmaxf(light.color.x, light.color.y), light.color.z);
	light.radius = (-light.linear + std::sqrtf(light.linear * light.linear - 4 * light.quadratic * (1.0 - (256.0 / 5.0) * lightMax))) / (2 * light.quadratic);

	global_debugLightShader = LoadShaderFromFile("shaders/DebugLights.vert", "shaders/DebugLights.frag");

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	LoadModel("models/containers/crate.obj");
	LoadModel("models/Pillars/pillar/pillar.obj");

    app.Run(MainLoop);
}
#endif