#include "World.hpp"

#include <fstream>

#include <Graphics/imgui.h>
#include <Engine/Assets.hpp>

namespace Raptor {

	//template<typename Component>
	//class ComponentAllocator {
	//	std::vector<ComponentArena<Component>*> arenas;
	//	Component& Alloc();
	//	void Free();
	//};

	//struct ComponentArena {
	//	U32* owners = nullptr;
	//	U32 count = 0;
	//	U32 capacity = 0;
	//	U8* memory = nullptr;

	//	ComponentArena(U32 componentCount, size_t componentSize);
	//};

	//// Create a component arena with the provided number of components
	//ComponentArena::ComponentArena(U32 componentCount, size_t componentSize) {
	//	memory = (U8*)malloc((componentCount * componentSize) + (componentCount * sizeof(U32)));
	//	owners = (U32*)(memory + (componentCount * componentSize));
	//	capacity = componentCount;
	//	count = 0;
	//}

	////An entity is just some number...

	//struct Entity {
	//	U32 id;
	//	U64 uuid;
	//};

	//Entity CreateEntity() {

	//}


void CreateEntity(World& world) {
	world.entityCount++;
}
void DestroyEntity(World& world) {
	world.entityCount--;
}

void UpdateWorld(World& world) {
}

void DrawEntities(World& world, GLuint shader) {
	Matrix4 modelMatrix = Matrix4::Identity();
	glUniformMatrix4fv(MODEL_LOCATION, 1, GL_FALSE, &modelMatrix[0][0]);

	Transform3D* transforms = (Transform3D*)world.components[COMPONENT_TRANSFORM];
	U32* modelIDs = (U32*)world.components[COMPONENT_MODELID];
	for (auto i = 0; i < world.entityCount; i++) {
		modelMatrix = TransformToMatrix(transforms[i]);
		glUniformMatrix4fv(MODEL_LOCATION, 1, GL_FALSE, &modelMatrix[0][0]);
		Draw(global_assetTable.models[modelIDs[i]]);
	}
}

void DrawLights(World& world, GLuint shader) {
	PointLight* pointLights = (PointLight*)world.components[COMPONENT_POINTLIGHT];
	for (auto i = 0; i < world.pointLightCount; i++) {
		PushLight(pointLights[i], i, shader);
	}
}

void RenderWorld(World& world, DeferredShader& shader, Camera& camera) {
	BeginDeferredShadingGeometryPass(shader, camera);
	DrawEntities(world, shader.geometeryPassProgram);
	EndDeferredShadingGeometeryPass();

	BeginDeferredShadingLightingPass(shader, camera);
	DrawLights(world, shader.lightingPassProgram);
	EndDeferredShadingLightingPass(shader);
}

void CreateTestWorld(World& world) {
	ComponentUsageInfo info;
	info.componentMemorySize[COMPONENT_TRANSFORM] = sizeof(Transform3D);
	info.initalComponentCount[COMPONENT_TRANSFORM] = 64;
		
	info.componentMemorySize[COMPONENT_MODELID] = sizeof(U32);
	info.initalComponentCount[COMPONENT_MODELID] = 64;
		 
	info.componentMemorySize[COMPONENT_POINTLIGHT] = sizeof(PointLight);
	info.initalComponentCount[COMPONENT_POINTLIGHT] = 64;

	size_t worldMemorySize = 0;
	for (U32 i = 0; i < COMPONENT_TYPE_COUNT; i++) {
		world.components[i] = (U8*)worldMemorySize;
		auto componentMemorySize = info.componentMemorySize[i] * info.initalComponentCount[i];
		worldMemorySize += componentMemorySize;
	}

	world.initalMemory = (U8*)malloc(worldMemorySize);
	for (auto i = 0; i < COMPONENT_TYPE_COUNT; i++) {
		world.components[i] = (U8*)((size_t)world.components[i] + (size_t)world.initalMemory);
	}
}


#if 0
void SaveWorld(const World& world, const std::string& filename) {
	std::ofstream stream(filename);
	if (!stream.is_open()) {
		LOG_ERROR("Could not open file " << filename << "when trying to save world");
		return;
	}

	stream.write((const char*)&world.entityCount, sizeof(U32));

	ComponentUsageInfo info;
	info.componentMemorySize[COMPONENT_TRANSFORM] = sizeof(Transform3D);
	info.initalComponentCount[COMPONENT_TRANSFORM] = 64;
	info.componentMemorySize[COMPONENT_MODELID] = sizeof(U32);
	info.initalComponentCount[COMPONENT_MODELID] = 64;
	info.componentMemorySize[COMPONENT_POINTLIGHT] = sizeof(PointLight);
	info.initalComponentCount[COMPONENT_POINTLIGHT] = 64;

	for (auto i = 0; i < COMPONENT_TYPE_COUNT; i++) {
		auto componentMemorySize = info.componentMemorySize[i] * info.initalComponentCount[i];
		stream.write((const char*)world.components[i], componentMemorySize);
	}
}

void LoadWorld(World& world, const std::string& filename) {
	std::ifstream stream(filename);
	if (!stream.is_open()) {
		LOG_ERROR("Could not open file " << filename << " when loading world");
		return;
	}

	ComponentUsageInfo info;
	info.componentMemorySize[COMPONENT_TRANSFORM] = sizeof(Transform3D);
	info.initalComponentCount[COMPONENT_TRANSFORM] = 64;
	info.componentMemorySize[COMPONENT_MODELID] = sizeof(U32);
	info.initalComponentCount[COMPONENT_MODELID] = 64;
	info.componentMemorySize[COMPONENT_POINTLIGHT] = sizeof(PointLight);
	info.initalComponentCount[COMPONENT_POINTLIGHT] = 64;

	stream.read((char*)&world.entityCount, sizeof(U32));

	size_t worldMemorySize = 0;
	for (U32 i = 0; i < COMPONENT_TYPE_COUNT; i++) {
		world.components[i] = (U8*)worldMemorySize;
		auto componentMemorySize = info.componentMemorySize[i] * info.initalComponentCount[i];
		worldMemorySize += componentMemorySize;
	}

	world.initalMemory = (U8*)malloc(worldMemorySize);
	for (auto i = 0; i < COMPONENT_TYPE_COUNT; i++) {
		world.components[i] = (U8*)((size_t)world.components[i] + (size_t)world.initalMemory);
		auto componentMemorySize = info.componentMemorySize[i] * info.initalComponentCount[i];
		stream.read((char*)world.components[i], componentMemorySize);
	}
}
#endif

#if 1
#include <SDL2/SDL.h>
void SaveWorld(const World& world, const std::string& filename) {
	SDL_RWops* file = SDL_RWFromFile(filename.c_str(), "w");
	SDL_RWwrite(file, &world.entityCount, sizeof(U32), 1);
	SDL_RWseek(file, sizeof(U32), RW_SEEK_CUR);

	ComponentUsageInfo info;
	info.componentMemorySize[COMPONENT_TRANSFORM] = sizeof(Transform3D);
	info.initalComponentCount[COMPONENT_TRANSFORM] = 64;
	info.componentMemorySize[COMPONENT_MODELID] = sizeof(U32);
	info.initalComponentCount[COMPONENT_MODELID] = 64;
	info.componentMemorySize[COMPONENT_POINTLIGHT] = sizeof(PointLight);
	info.initalComponentCount[COMPONENT_POINTLIGHT] = 64;

	for (auto i = 0; i < COMPONENT_TYPE_COUNT; i++) {
		SDL_RWwrite(file, world.components[i], info.componentMemorySize[i], info.initalComponentCount[i]);
		SDL_RWseek(file, info.componentMemorySize[i] * info.initalComponentCount[i], RW_SEEK_CUR);
	}

	SDL_RWclose(file);
}

void LoadWorld(World& world, const std::string& filename) {
	assert(world.initalMemory == nullptr);
	SDL_RWops* file = SDL_RWFromFile(filename.c_str(), "r");
	SDL_RWread(file, &world.entityCount, sizeof(U32), 1);
	SDL_RWseek(file, sizeof(U32), RW_SEEK_CUR);

	ComponentUsageInfo info;
	info.componentMemorySize[COMPONENT_TRANSFORM] = sizeof(Transform3D);
	info.initalComponentCount[COMPONENT_TRANSFORM] = 64;
	info.componentMemorySize[COMPONENT_MODELID] = sizeof(U32);
	info.initalComponentCount[COMPONENT_MODELID] = 64;
	info.componentMemorySize[COMPONENT_POINTLIGHT] = sizeof(PointLight);
	info.initalComponentCount[COMPONENT_POINTLIGHT] = 64;

	size_t worldMemorySize = 0;
	for (auto i = 0; i < COMPONENT_TYPE_COUNT; i++) {
		world.components[i] = (U8*)worldMemorySize;
		worldMemorySize += info.componentMemorySize[i] * info.initalComponentCount[i];
	}

	world.initalMemory = (U8*)malloc(worldMemorySize);
	for (auto i = 0; i < COMPONENT_TYPE_COUNT; i++) {
		world.components[i] = (U8*)((size_t)world.components[i] + (size_t)world.initalMemory);
		void* components = world.components[i];
		SDL_RWread(file, components, info.componentMemorySize[i], info.initalComponentCount[i]);
		SDL_RWseek(file, info.componentMemorySize[i] * info.initalComponentCount[i], RW_SEEK_CUR);
	}

	world.pointLightCount = 64;
	SDL_RWclose(file);
}


#endif

};