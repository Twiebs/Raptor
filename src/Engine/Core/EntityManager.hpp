#pragma once

#include <algorithm>
#include <unordered_map>
#include <typeindex>

#if _DEBUG
#include<type_traits>
#endif

#include <Core/IService.hpp>

#include <Core/Common.hpp>
#include <Core/Component.hpp>
#include <Core/ComponentBlock.hpp>
#include <Core/ComponentType.hpp>
#include <Core/Entity.hpp>
#include <Core/ISystem.hpp>

#include <Utils/UnorderedArray.hpp>

#define MAX_COMPONENTS 64

#ifdef _DEBUG
#define SYSTEM_CREATED systemCreated++;
#define SYSTEM_DESTROYED
#define COMPONET_CREATED
#endif
#ifndef _DEBUG
#define SYSTEM_CREATED
#define SYSTEM_DESTROYED
#define COMPONET_CREATED
#endif

class EntityManager: IService {
public:
	EntityManager();
	~EntityManager();

	//TODO move to a delayed opperation handling scheme... Opperations should be delayed until the next tick of the engine
	// Creates a new system and adds it to the entityManager
	// Returns a raw pointer to the system that was just created
	template<typename T, typename ... Args>
	T* CreateSystem(Args ... args) {
		static_assert(std::is_base_of<ISystem, T>::value, "Created systems must inherit from the System interface!");
		systems.emplace_back(new T(args...));
		ISystem* system = systems[systems.size() - 1];
		system->manager = this;
		system->engine = engine;
		bool success = system->Startup(this);
		return (T*) system;
	}

	//This opperation should be delayed until the next Tick of the engine
	//Destroys a system in the engine
	template<typename T>
	void DestroySystem();

	//Entity stuff
	EntityID CreateEntity();
	Entity* GetEntity(EntityID id);

	void RemoveEntity(Entity* entity);

	//Creates a component or reused one from a pool
	//Creates a component and stores a ptr to it by the entityID
	template<typename T, typename ... Args>
	T* CreateComponent(EntityID entityID, Args ... args) {
		static_assert(std::is_base_of<Component, T>::value, "You are trying to create a component that does not inherit from the base component interface!");
		ComponentType* componentType = ComponentTypeOf<T>();
		ComponentBlock* componentBlock = componentBlocks[componentType->index];
		T* component = (T*) componentBlock->Alloc<T>(args...);
		component->ownerID = entityID;
		uint32 componentIndex = componentBlock->Count();//Count is on purpose
		//We set the componentIndex to +1 of the actual index so that 0 represents that the entity does not have a component of that type
		componentsByEntityID[componentType->index].Set(entityID,
				componentIndex);
		return component;
	}

	//Gets a component from an entity id
	template<typename T>
	T* GetComponent(EntityID id) {
		static_assert(std::is_base_of<Component, T>::value, "You are trying to get a component that does not inherit from the base component interface!");
		ComponentType* componentType = ComponentTypeOf<T>();
		ComponentBlock* componentBlock = componentBlocks[componentType->index];
		uint32 componentIndex = componentsByEntityID[componentType->index].Get(
				id);
		if (componentIndex == 0) {
			//The entity does not have this component
			return nullptr;
		}
		T* component = (T*) componentBlock->Get(componentIndex - 1);//Subtract one since the component index is offset
		return component;
	}

	template<typename T>
	void RegisterComponent(uint64 flags) {
		static_assert(std::is_base_of<Component, T>::value, "You can not register a component that does not inherit from the base component class!");
		const std::type_info& typeInfo = typeid(T);	//TODO make sure that components are only registered once!
		componentTypes.emplace_back(ComponentType { flags, nextComponentTypeBit,
				nextComponentTypeIndex, sizeof(T) });
		componentTypeIndexMap[typeInfo] = nextComponentTypeIndex;
		auto type = &componentTypes[nextComponentTypeIndex];
		nextComponentTypeBit << 1;
		nextComponentTypeIndex += 1;

		ComponentBlock* block = componentBlocks[type->index];
		//Block was already registered with the engine!
		block = new ComponentBlock(type);
		componentBlocks[type->index] = block;
	}

	template<typename T>
	void RemoveComponent(EntityID id) {

	}

	//static ComponentType& GetComponentType(const std::type_info& typeInfo);
	//static ComponentGroup& GetComponetGroup(std::bitset<MAX_GROUPS> bits);

	template<typename T>
	ComponentType* ComponentTypeOf() {
		//Make sure the user is providing an actual component
		static_assert(std::is_base_of<Component, T>::value, "Type param must be derrived from Component!");
		const std::type_info& typeInfo = typeid(T);
		uint32 typeIndex = componentTypeIndexMap[typeInfo];
		ComponentType* type = &componentTypes[typeIndex];
		//Component types must be registered before use!
		return type;
	}

	ComponentBlock* GetComponentBlock(uint32 index) const;

	std::vector<Entity*>* GetEntities();
	std::vector<Entity*>* GetEntities(std::bitset<MAX_COMPONENTS>);

	void Update(double deltaTime);

private:
	//Any entitiy with an ID of zero was not created properly
	EntityID nextEntityID = 1;//The entity id is the index of the created entitiy.  These are recycled
	EntityUUID nextEntityUUID = 1;//This is a unique identifier everytime a entitiy is recycled or created

	std::vector<ISystem*> systems;//Array of systems that have been created by the engine
	std::vector<Entity> entities;//Array of all the entities ever created by the manager indexed by thier entity.id
	std::vector<EntityID> removedEntities;//Array of entities that have been removed and are awaiting recylcing
	std::vector<ComponentBlock*> componentBlocks;

	std::vector<UnorderedArray<uint32>> componentsByEntityID;

	//Component Type
	std::bitset<MAX_COMPONENTS> nextComponentTypeBit; //The bit the next registered ComponentType will have
	uint32 nextComponentTypeIndex = 0; //The index the next registered ComponentType will have
	std::vector<ComponentType> componentTypes; //This is where the component types are stored in memory
	std::unordered_map<std::type_index, uint32> componentTypeIndexMap; //The map of component types

	friend class EntitySystem;
	friend class Engine;

public:
#if _DEBUG
	uint64 entitiesActive;		//Count of active entities
	uint64 entitiesDisabled;//Count of the currently disabled entities
	uint64 entitiesAdded;//Total amount of entites that were added during the lifetime of the manager
	uint64 entitiesRemoved;//Total number of entites that were removed during the lifetime of the manager
	uint64 entitiesCreated;//Total amount of entities ever created by the manager
	uint64 entitiesDeleted;//Total number of entities ever deleted by the manager

	uint64 componentsCreated;//Total number of components created by the manager
	uint64 componentsDeleted;//Total number of components that were deleted by the engine
	uint64 componentsActive;//Total number of components being used by entities
	uint64 componentsDisabled;//Total number of components that are not being used by an entity

	uint64 systemsActive;//Count of the systems currently being updated within the manager
	uint64 sytemsDisabled;//Count of systems in the manager that are not being updated by the manager
	uint64 systemsCreated;//Count of all the systems created by the manager
	uint64 systemsDeleted;//Count of all the systems deleted by the manager
#endif

#ifdef BENCHMARK
	double lastUpdateTime;
#endif

};
