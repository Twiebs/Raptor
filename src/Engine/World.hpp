#pragma once

#include <Graphics/Render3D.hpp>

namespace Raptor {

#define MAX_ENTITIES 256

enum ComponentType {
	COMPONENT_TRANSFORM,
	COMPONENT_MODELID,
	COMPONENT_POINTLIGHT,
	COMPONENT_TYPE_COUNT
};

enum EntityFlags {
	ENTITY_REMOVAL	= 1 << 0,
	ENTITY_SELECTED = 1 << 1,
	ENTITY_INACTIVE = 1 << 2,
};

struct ComponentTypeInfo {
	size_t componentMemorySize;
	U32 initalComponentCount;
};

struct ComponentUsageInfo {
	size_t componentMemorySize[COMPONENT_TYPE_COUNT];
	U32 initalComponentCount[COMPONENT_TYPE_COUNT];
};

struct World {
	U8* components[COMPONENT_TYPE_COUNT];
	U32 pointLightCount;
	U32 entityCount;
	U8* initalMemory;

	U32 entityFlags[64] = {};
};

void CreateTestWorld(World& world);

void UpdateWorld(World& world);
void RenderWorld(World& world, DeferredShader& shader, Camera& camera);
void RenderWorld(World& world, FowardShader& shader, Camera& camera);

void CreateEntity(World& world);
void DestroyEntity(World& world);

void SaveWorld(const World& world, const std::string& filename);
void LoadWorld(World& world, const std::string& filename);

}
