
#include "ECSManager.hpp"

ECSManager::ECSManager() :
	componentsByEntityID(MAX_COMPONENTS, UnorderedArray<uint32>()),
	nextComponentTypeBit(1) { }

ECSManager::~ECSManager() {
	for(ISystem* system : systems)
		delete system;
	delete[] componentBlocks;
}

Entity ECSManager::CreateEntity() {
	if (removedEntities.size() > 0) {
		auto entityID = removedEntities[removedEntities.size() - 1];
		removedEntities.pop_back();
		entities[entityID].uuid = nextEntityUUID++;
		return entities[entityID];
	}

	Entity entity;
	entity.id = nextEntityID++;
	entity.uuid = nextEntityUUID++;
	entities.push_back(entity);
	return entity;
}

//TODO also create default entity size etc...
//which is actually meaningless....
//Allocate something truly enormous here...
void ECSManager::Initalize() {
	componentBlocks = new ComponentBlock[componentTypeCount];
	assert(componentTypes.size() == componentTypeCount);
	for(uint32 i = 0; i < componentTypeCount; i++) {
		componentBlocks[i].SetDataSize(componentTypes[i].size);
	}
}

ComponentBlock* ECSManager::GetComponentBlock(uint32 index) const {
	return &componentBlocks[index - 1];
}

//FIXME now broken...
void ECSManager::RemoveEntity(Entity entity) {
//	//TODO free entities components
//	entity->componentBits = 0;
//	entity->uuid = 0;	//UUID of zero is not provided by the engine
//	//RefreshEntityComponentMask(entity);
//	removedEntities.push_back(entity);	//Add the entity to the list of removedEntities
//#if DEBUG
//	active--;
//	removed++;
//	disabled++;
//#endif
}
//
//void ECSManager::RefreshEntityComponentMask(Entity* entity) {
//	for (auto iter = componentGroupMap.begin(); iter != componentGroupMap.end(); iter++) {
//		ComponentGroup& group = *iter->second;
//		bool isInGroup = ((entity->groupBits & group.bit) == group.bit);
//		bool matchesGroup = ((entity->componentBits & group.componentMask) == group.componentMask);
//
//		if (!isInGroup && matchesGroup) {
//			std::vector<Entity*>*  groupedEntities = componentGroupedEntities[group.index];
//			groupedEntities->push_back(entity);
//			entity->groupBits |= group.bit;
//		}
//		else if (isInGroup && !matchesGroup) {
//			std::vector<Entity*>*  groupedEntities = componentGroupedEntities[group.index];
//			groupedEntities->erase(std::remove(groupedEntities->begin(), groupedEntities->end(), entity));
//			entity->groupBits ^= group.bit;
//		}
//	}
//}



//Gets or creates a new array with entities that contain the components
//std::vector<Entity*>* ECSManager::GetEntities(std::bitset<MAX_COMPONENTS> componentMaskBits) {
//
//	ComponentGroup& group = GetComponetGroup(componentMaskBits);
//
//	if (componentGroupedEntities.size() <= group.index){
//		componentGroupedEntities.push_back(new std::vector<Entity*>);
//		for (Entity* entity : entities) {
//			RefreshEntityComponentMask(entity);
//		}
//	}
//	std::vector<Entity*>* groupedEntities= componentGroupedEntities[group.index];
//	return groupedEntities;
//}

//Returns a reference to a component type from a map of type_infos to ComponetTypes
//Or Initalizes a new instance
//TODO: Consider Pre-Registration of compnonents with #define
//ComponentType& ECSManager::GetComponentType(const std::type_info& typeInfo) {
//	ComponentType* type = componentTypes[typeInfo];
//	//The component type has not been registered with the manager yet
//	if (type == nullptr) {
//		//Create the new ComponentType and set its bit and index
//		type = new ComponentType {
//			nextComponentTypeBit,
//			nextComponentTypeIndex,
//		};
//		//Set the ComponentType Pointer
//		componentTypes[typeInfo] = type;
//		//The maskBit and index are incremented for the nextComponentType
//		nextComponentTypeBit = nextComponentTypeBit << 1;
//		nextComponentTypeIndex += 1;
//	}
//	//Return a refrence to the retrieved or created component type
//	return *type;
//}

//ComponentGroup& ECSManager::GetComponetGroup(std::bitset<MAX_GROUPS> bits) {
//	ComponentGroup* group = componentGroupMap[bits];
//	if (group == nullptr) {
//		group = new ComponentGroup{
//			bits,
//			nextComponentGroupBit,
//			nextComponentGroupIndex
//		};
//		componentGroupMap[bits] = group;
//		nextComponentGroupBit = nextComponentGroupBit << 1;
//		nextComponentTypeIndex += 1;
//	}
//	return *group;
//}


void ECSManager::Update(double deltaTime) {
#ifdef BENCHMARK
	static double logTime = 0;
	auto start = std::chrono::steady_clock::now();
#endif

	for (auto& system : systems) {
		system->Update(deltaTime);
#ifdef BENCHMARK
		if (logTime > 1.0) {
			std::cout << system->tag << ": " << system->lastUpdateTime << "\n";
		}
#endif
	}

#ifdef BENCHMARK
	auto end = std::chrono::steady_clock::now();
	auto diff = end - start;
	auto duration = std::chrono::duration<double, std::milli>(diff).count();
	lastUpdateTime = duration;

	if (logTime > 1.0) {
		std::cout << "Engine: " << lastUpdateTime << "\n";
		std::cout << "\n";
		logTime = 0;
	}
	logTime += deltaTime;
#endif

	//for (auto& componentType : componentTypes) {
	//	if (componentType.flags & (uint64)ComponentTypeFlag::UseTransform) {
	//		uint32 count = componentBlocks[componentType.index]->Count();

	//		for (componentBlocks[componentType.index])
	//	}
	//}
}
