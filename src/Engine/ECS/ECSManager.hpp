#pragma once

#include <typeindex>
#include <unordered_map>
#include <vector>
#include <assert.h>
#include <memory>

#if _DEBUG
#include<type_traits>
#endif

#include <Core/Common.hpp>
#include <ECS/Component.hpp>
#include <ECS/ComponentBlock.hpp>
#include <ECS/ComponentType.hpp>
#include <ECS/Entity.hpp>
#include <ECS/ISystem.hpp>

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

//NOTE
// - Components should not hold any data...
// - They simply should just point to the data that they are referencing...

//TODO Consider moving template functions into an .inl header
//This could increase the readability of this file considerably


//NOTE Why do we even bother inheriting from some service thing?
//it serves no value whatsoever
class ECSManager{
public:
	ECSManager();
	~ECSManager();

	//TODO move to a delayed ropperation handling scheme... Opperations should be delayed until the next tick of the engine
	// Creates a new system and adds it to the entityManager
	// Returns a raw pointer to the system that was just created
	template<typename T, typename ... Args>
	T* CreateSystem(Args ... args) {
		static_assert(std::is_base_of<ISystem, T>::value, "Created systems must inherit from the System interface!");
		systems.emplace_back(new T(args...));
		ISystem* system = systems[systems.size() - 1];
		system->manager = this;
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
		ComponentBlock* componentBlock = &componentBlocks[componentType->index - 1];
		T* component = (T*) componentBlock->Alloc<T>(args...);
		component->ownerID = entityID;
		uint32 componentIndex = componentBlock->Count();//Count is on purpose
		//We set the componentIndex to +1 of the actual index so that 0 represents that the entity does not have a component of that type
		componentsByEntityID[componentType->index - 1].Set(entityID,
				componentIndex);
		return component;
	}

	//Gets a component from an entity id
	template<typename T>
	T* GetComponent(EntityID id) {
		static_assert(std::is_base_of<Component, T>::value, "You are trying to get a component that does not inherit from the base component interface!");
		ComponentType* componentType = ComponentTypeOf<T>();
		ComponentBlock* componentBlock = &componentBlocks[componentType->index - 1];
		uint32 componentIndex = componentsByEntityID[componentType->index - 1].Get(
				id);
		if (componentIndex == 0) {
			//The entity does not have this component
			return nullptr;
		}
		T* component = (T*) componentBlock->Get(componentIndex - 1);//Subtract one since the component index is offset
		return component;
	}

	//TODO make sure that components are only registered once!
	template<typename T>
	void RegisterComponent(uint64 flags) {
		static_assert(std::is_base_of<Component, T>::value, "You can not register a component that does not inherit from the base component class!");
		assert(componentTypeIndexMap[typeid(T)] == 0);	//Ensure the component has not been registered already

		const std::type_info& typeInfo = typeid(T);
		componentTypes.emplace_back(ComponentType { flags, nextComponentTypeBit, ++componentTypeCount, sizeof(T) });
		componentTypeIndexMap[typeInfo] = componentTypeCount;
		nextComponentTypeBit << 1;
	}

	template<typename T>
	void RemoveComponent(EntityID id) {

	}


	template<typename T>
	ComponentType* ComponentTypeOf() {
		static_assert(std::is_base_of<Component, T>::value, "Type param must be derrived from Component!");

		const std::type_info& typeInfo = typeid(T);
		uint32 typeIndex = componentTypeIndexMap[typeInfo];
		assert(typeIndex != 0);	//TypeIndices must not be zero!  If the typeIndex is zero it has not been registered with the manager yet!

		ComponentType* type = &componentTypes[typeIndex - 1];
		return type;
	}

	ComponentBlock* GetComponentBlock(uint32 index) const;

	std::vector<Entity*>* GetEntities();
	std::vector<Entity*>* GetEntities(std::bitset<MAX_COMPONENTS>);

	void Initalize();

	void Update(double deltaTime);

private:
	//Any entity with an ID of zero was not created properly
	EntityID nextEntityID = 1;	//The entity id is the index of the created entity.  These are recycled
	EntityUUID nextEntityUUID = 1;	//This is a unique identifier every time a entity is recycled or created

	std::vector<ISystem*> systems;	//Array of systems that have been created by the engine

	//??? Why do we even care about what entities are??? At this point the only purpose they serve is to be indexes into components
	// And essentially a way to retrieve those components associated with that entity
	// If we do want to eventually go the entitySystem / Family route there might be a better way to determine the families that an entity belongs to...
	// Instead of keeping that information inside the entity the information can be packed into a different array that holds the component bits of each entity
	// indexed by the entities id.  This way iteration could happen over each bitset inside the array in order to iterate through all the entities...
	// In order to determine if an entity is still valid in the world its uuid can be checked against its id to see if the entity is actually still participating in the world
	// I'm going to do this for now in order to simplify this implementation further

	//NOTE we could also do something where component blocks don't reside in a vector... we already know how many of them there should be so there is no reason to actually have the vector.

	//Also why store componentBlockPtrs? we could allocate them by value... there is no reason not to...

	std::vector<Entity> entities;					//Array of all the entities ever created by the manager indexed by their entity.id
	std::vector<EntityID> removedEntities;			//Array of entities that have been removed and are awaiting recycling
	ComponentBlock* componentBlocks;

	std::vector<UnorderedArray<uint32>> componentsByEntityID;

	//Component Type
	uint32 componentTypeCount = 0;
	std::bitset<MAX_COMPONENTS> nextComponentTypeBit; 					//The bit the next registered ComponentType will have
	//uint32 nextComponentTypeIndex = 1; 									//The index the next registered ComponentType will have
	std::vector<ComponentType> componentTypes; 							//This is where the component types are stored in memory
	std::unordered_map<std::type_index, uint32> componentTypeIndexMap; 	//The map of component types

public:
#if _DEBUG
	uint64 entitiesActive;		//Count of active entities
	uint64 entitiesDisabled;	//Count of the currently disabled entities
	uint64 entitiesAdded;		//Total amount of entities that were added during the lifetime of the manager
	uint64 entitiesRemoved;		//Total number of entities that were removed during the lifetime of the manager
	uint64 entitiesCreated;		//Total amount of entities ever created by the manager
	uint64 entitiesDeleted;		//Total number of entities ever deleted by the manager

	uint64 componentsCreated;	//Total number of components created by the manager
	uint64 componentsDeleted;	//Total number of components that were deleted by the engine
	uint64 componentsActive;	//Total number of components being used by entities
	uint64 componentsDisabled;	//Total number of components that are not being used by an entity

	uint64 systemsActive;		//Count of the systems currently being updated within the manager
	uint64 sytemsDisabled;		//Count of systems in the manager that are not being updated by the manager
	uint64 systemsCreated;		//Count of all the systems created by the manager
	uint64 systemsDeleted;		//Count of all the systems deleted by the manager
#endif

#ifdef BENCHMARK
	double lastUpdateTime;
#endif

};
