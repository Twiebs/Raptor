
#include <ECS/EntityManager.hpp>
#include<chrono>

EntityManager::EntityManager() : 
	componentBlocks(MAX_COMPONENTS, nullptr),
	nextComponentTypeBit(1) { }

EntityManager::~EntityManager() {

}

//TODO: Entity pooling
Entity* EntityManager::CreateEntity() {
	Entity* entity;

	if (removedEntities.size() > 0) {
		Entity* entity = removedEntities[removedEntities.size() - 1];
		removedEntities.pop_back();

		//We retrieve an entity from the removed entities but the id is reused
		//The uuid is allways unique
		entity->uuid = nextEntityUUID++;

		#if DEBUG
			added++;
			active++;
			disabled--;
		#endif
		return entity;
	}

	entity = new Entity(this, nextEntityUUID++, nextEntityID++);
	entities.push_back(entity);
#if DEBUG
	created++;
	addded++;
	active++;
#endif
	return entity;
}

ComponentBlock* EntityManager::GetComponentBlock(uint32 index) const {
	return componentBlocks[index];
}


void EntityManager::RemoveEntity(Entity* entity) {
	//TODO free entities components
	entity->componentBits = 0;
	entity->uuid = 0;	//UUID of zero is not provided by the engine
	//RefreshEntityComponentMask(entity);
	removedEntities.push_back(entity);	//Add the entity to the list of removedEntities
#if DEBUG
	active--;
	removed++;
	disabled++;
#endif
}
//
//void EntityManager::RefreshEntityComponentMask(Entity* entity) {
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

//Returns the actual array with the entities in it
std::vector<Entity*>* EntityManager::GetEntities() {
	return &entities;
}

//Gets or creates a new array with entities that contain the components
//std::vector<Entity*>* EntityManager::GetEntities(std::bitset<MAX_COMPONENTS> componentMaskBits) {
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
//ComponentType& EntityManager::GetComponentType(const std::type_info& typeInfo) {
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

//ComponentGroup& EntityManager::GetComponetGroup(std::bitset<MAX_GROUPS> bits) {
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


void EntityManager::Update(float deltaTime) {
#ifdef BENCHMARK
	static double logTime = 0;
	auto start = std::chrono::steady_clock::now();
#endif

	for (ISystem* system : systems) {
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
}
