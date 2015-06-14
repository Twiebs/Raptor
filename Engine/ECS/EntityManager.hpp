#pragma once

#include<algorithm>
#include<map>
#include<unordered_map>
#include<typeindex>

#if _DEBUG
//#define options
#include<type_traits>
#endif	

#include <Core\Config.hpp>

#include <ECS\Bag.hpp>
#include <ECS\Component.hpp>
#include <ECS\ComponentBlock.hpp>
#include <ECS\ComponentType.hpp>
#include <ECS\Entity.hpp>
#include <ECS\ISystem.hpp>


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


class EntityManager {
public:
	EntityManager();
	~EntityManager();

	//TODO move to a delayed opperation handling scheme... Opperations should be delayed until the next tick of the engine
	// Creates a new system and adds it to the entityManager
	// Returns a raw pointer to the system that was just created
	template <typename T, typename... Args>
	T* CreateSystem(Args... args) {
		static_assert(std::is_base_of<ISystem, T>::value, "Created systems must inherit from the System interface!");
		systems.emplace_back(std::make_unique<T>(args...));
		auto& system = systems[systems.size() - 1];
		system->AddedToManager(this);
		system->Startup(this);
		return (T*)system.get();
	}

	//This opperation should be delayed until the next Tick of the engine
	//Destroys a system in the engine
	template <typename T>
	void DestroySystem();


	//Entity stuff
	EntityID CreateEntity();
	Entity& GetEntity(EntityID id);

	void RemoveEntity(Entity* entity);
	//void RefreshEntityComponentMask(Entity* entity);

	//Creates a component or reused one from a pool

	template<typename T, typename... Args>
	T* CreateComponent(Entity* entity, Args... args) {
		//Get the type of component and its block
		ComponentType* type = ComponentTypeOf<T>();
		ComponentBlock* componentBlock = componentBlocks[type->index];

		if (componentBlock == nullptr) {
			componentBlock = new ComponentBlock(type);
			componentBlocks[type->index] = componentBlock;
		}

		T* component = (T*)componentBlock->Alloc<T>();
		entity->componentBits |= type->bit;
#if _DEBUG
		componentsCreated++;
#endif
		return component;
	}

	template<typename T>
	void RemoveComponent() {

	}

	//static ComponentType& GetComponentType(const std::type_info& typeInfo);
	//static ComponentGroup& GetComponetGroup(std::bitset<MAX_GROUPS> bits);


	template<typename T>
	ComponentType* ComponentTypeOf() {
		//Make sure the user is providing an actual component
		static_assert(std::is_base_of<Component, T>::value, "Type param must be derrived from Component!");
		const std::type_info& typeInfo = typeid(T);
		ComponentType* type = componentTypes[typeInfo];

		//This component has not been registered with the engine yet
		if (type == nullptr) {
			type = new ComponentType { nextComponentTypeBit, nextComponentTypeIndex, sizeof(T) };
			componentTypes[typeInfo] = type;
			nextComponentTypeBit << 1;
			nextComponentTypeIndex += 1;
		}
		return type;
	}

	ComponentBlock* GetComponentBlock(uint32 index) const;

	std::vector<Entity*>* GetEntities();
	std::vector<Entity*>* GetEntities(std::bitset<MAX_COMPONENTS>);

	void Update(float deltaTime);

private:
	//Any entitiy with an ID of zero was not created properly 
	EntityID   nextEntityID	  =	1;	//The entity id is the index of the created entitiy.  These are recycled
	EntityUUID nextEntityUUID = 1;	//This is a unique identifier everytime a entitiy is recycled or created

	std::vector<std::unique_ptr<ISystem>> systems;			//Array of systems that have been created by the engine
	std::vector<Entity> entities;			//Array of all the entities ever created by the manager indexed by thier entity.id
	std::vector<EntityID> removedEntities;	//Array of entities that have been removed and are awaiting recylcing
	std::vector<ComponentBlock*> componentBlocks;

	//Component Type
	std::bitset<MAX_COMPONENTS> nextComponentTypeBit; //The bit the next registered ComponentType will have
	uint32 nextComponentTypeIndex;	//The index the next registered ComponentType will have
	std::unordered_map<std::type_index, ComponentType*> componentTypes;	//The map of component types

	friend class ISystem;
	friend class EntitySystem;

public:
#if _DEBUG
	uint64 entitiesActive;		//Count of active entities
	uint64 entitiesDisabled;	//Count of the currently disabled entities
	uint64 entitiesAdded;		//Total amount of entites that were added during the lifetime of the manager
	uint64 entitiesRemoved;		//Total number of entites that were removed during the lifetime of the manager
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

