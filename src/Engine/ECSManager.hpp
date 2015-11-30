#ifndef RAPTOR_ECSMANAGER_HPP
#define RAPTOR_ECSMANAGER_HPP

#include <Core/logging.h>
#include <Core/types.h>

#include <cstring>
#include <functional>
#include <Utils/Metafuck.hpp>

#include <Core/Tasks.hpp>

struct Entity {
	U32 id;
	U64 uuid;
	Entity() { }
	Entity(U32 id, U64 uuid) : id(id), uuid(uuid) { }
};

#define ASSERT_COMPONENT_TYPE_IS_VALID() static const size_t __valid_type_check = IndexOfType<TComponent, TComponentList>::value

template<typename TComponentList>
class ECSManager {

	struct ComponentAllocator {
		std::vector<U32> owners;
		size_t count;
		size_t capacity;
		U8* data;
	} componentAllocators[TComponentList::Size()];

public:
	ECSManager(U32 initalEntityCount);

	Entity& CreateEntity();
	void DestroyEntity(const Entity& entity);

	template <typename TComponent, typename... TArgs>
	TComponent* CreateComponent (const Entity &entity, TArgs... args) {
		ASSERT_COMPONENT_TYPE_IS_VALID();
		const size_t componentTypeindex = IndexOfType<TComponent, TComponentList>::value;
		ComponentAllocator* allocator = &componentAllocators[componentTypeindex];
		if (allocator->count + 1 > allocator->capacity) {
			size_t newCapacity = allocator->capacity + 30;
			U8* newData = static_cast<U8*>(malloc(sizeof(TComponent) * newCapacity));
			memcpy(newData, allocator->data, sizeof(TComponent) * allocator->count);
			free(allocator->data);
			allocator->data = newData;
			allocator->capacity = newCapacity;
			allocator->owners.reserve(newCapacity);
		}

		TComponent* component = reinterpret_cast<TComponent*>(allocator->data + (sizeof(TComponent) * allocator->count));
		new (component) TComponent(args...);
		allocator->owners.emplace_back(entity.id);
		allocator->count += 1;
		auto& entityComponentIndices = componentIndicesForEntity[entity.id];
		entityComponentIndices[componentTypeindex] = allocator->count - 1;
		return component;
	}


	template <typename TComponent>
	void DestroyComponent (const Entity& entity) {
		ASSERT_COMPONENT_TYPE_IS_VALID();
		assert(entity.uuid != 0 && "This entity has been already been removed from the manager");
		const size_t componentTypeIndex = IndexOfType<TComponent, TComponentList>::value;

		auto& componentIndices = componentIndicesForEntity[entity.id];
		size_t indexOfComponentToRemove = componentIndices[componentTypeIndex];
		assert(indexOfComponentToRemove != -1 && "Entity does not own this component type");

		ComponentAllocator* allocator = &componentAllocators[componentTypeIndex];
		TComponent* componentToDestroy = reinterpret_cast<TComponent*>(allocator->data + (sizeof(TComponent) * indexOfComponentToRemove));
		TComponent* componentToMove = reinterpret_cast<TComponent*>(allocator->data + (sizeof(TComponent) * (allocator->count - 1)));
		componentToDestroy->~TComponent();
		memcpy(componentToDestroy, componentToMove, sizeof(TComponent));

		U32 ownerOfMovedComponent = allocator->owners.back();
		allocator->owners.pop_back();
		allocator->count--;
		allocator->owners[indexOfComponentToRemove] = ownerOfMovedComponent;

		auto& componentIndicesOfMovedComponentOwner = componentIndicesForEntity[ownerOfMovedComponent];
		componentIndicesOfMovedComponentOwner[componentTypeIndex] = ownerOfMovedComponent;
	}

	template <typename TComponent>
	TComponent* GetComponent (const Entity& entity) {
		ASSERT_COMPONENT_TYPE_IS_VALID();
		const size_t componentTypeIndex = IndexOfType<TComponent, TComponentList>::value;
		ComponentAllocator* allocator = &componentAllocators[componentTypeIndex];
		auto& componentIndices = componentIndicesForEntity[entity.id];
		TComponent* component = reinterpret_cast<TComponent*>(allocator->data + (sizeof(TComponent) * componentIndices[componentTypeIndex]));
		return component;
	}


	template<typename TComponent>
	void ProcessComponent (std::function<void(const Entity&, TComponent* component)> procedure) {
		ASSERT_COMPONENT_TYPE_IS_VALID();
		const size_t componentTypeIndex = IndexOfType<TComponent, TComponentList>::value;
		ComponentAllocator* allocator = &componentAllocators[componentTypeIndex];
		TComponent* componentPointer = reinterpret_cast<TComponent*>(allocator->data);
		for (U32 i = 0; i < allocator->count; i++) {
			auto& entity = entities[allocator->owners[i]];
			procedure(entity, componentPointer);
			componentPointer += 1;
		}
	}

	//template<typename TComponent>
	//void ProcessComponent (std::function<void(TComponent*)> procedure) {
	//	ASSERT_COMPONENT_TYPE_IS_VALID();
	//	const size_t componentTypeIndex = IndexOfType<TComponent, TComponentList>::value;
	//	ComponentAllocator* allocator = &componentAllocators[componentTypeIndex];
	//	TComponent* componentPointer = reinterpret_cast<TComponent*>(allocator->data);
	//	for (U32 i = 0; i < allocator->count; i++) {
	//		procedure(componentPointer);
	//		componentPointer += 1;
	//	}
	//}

	private:
	friend class ComponentManager;

	template<typename TComponentType, int N>
	struct InitalizeComponentAllocators;

	template<typename THead, typename... TTypeList, int N>
	struct InitalizeComponentAllocators<TypeList<THead, TTypeList...>, N> {
		static void Initalize(ECSManager<TComponentList>* manager) {
			ComponentAllocator* allocator = &manager->componentAllocators[N];
			allocator->count = 0;
			allocator->capacity = 128;
			allocator->data = static_cast<U8*>(malloc(sizeof(THead) * 128));
			allocator->owners.reserve(128);
			InitalizeComponentAllocators<TypeList<TTypeList...>, N + 1>::Initalize(manager);
		}
	};

	template<int N>
	struct InitalizeComponentAllocators<TypeList<>, N> {
		static void Initalize(ECSManager<TComponentList>* manager) { }
	};


	template<typename TCompnentType, int N>	struct EntityDestructor;
	template<int N> struct EntityDestructor<TypeList<>, N> { 
		static void DestroyComponent(ECSManager<TComponentList>* manager, const Entity& entity) { }
	};

	template<typename TFirst, typename... TTypeList, int N>
	struct EntityDestructor<TypeList<TFirst, TTypeList...>, N> {
		static void DestroyComponent(ECSManager<TComponentList>* manager, const Entity& entity) {
			const size_t componentTypeIndex = IndexOfType<TFirst, TComponentList>::value;
			auto entityComponentIndexForType = manager->componentIndicesForEntity[entity.id][componentTypeIndex];
			if (entityComponentIndexForType != -1) {
				manager->DestroyComponent<TFirst>(entity);
			}
			EntityDestructor<TypeList<TTypeList...>, N + 1>::DestroyComponent(manager, entity);
		}
	};

public:
	std::vector<Entity> entities;
	std::vector<U32> removedEntites;
	std::vector<std::vector<S32>> componentIndicesForEntity;

private:
	U32 nextUUID = 1;
};

template <typename TComponentList>
ECSManager<TComponentList>::ECSManager(U32 initalEntityCount) {
	entities.reserve(initalEntityCount);
	removedEntites.reserve(initalEntityCount);
	componentIndicesForEntity.reserve(initalEntityCount);
	InitalizeComponentAllocators<TComponentList, 0>::Initalize(this);
}

template<typename TComponentList>
Entity& ECSManager<TComponentList>::CreateEntity() {
	if (removedEntites.size() > 0) {
		auto entityIndex = removedEntites.back();
		removedEntites.pop_back();
		auto& entity = entities[entityIndex];
		entity.uuid = nextUUID++;
		return entity;
	}

	entities.emplace_back(entities.size(), nextUUID++);
	componentIndicesForEntity.emplace_back(TComponentList::Size(), -1);
	auto& entity = entities.back();
	return entity;
}

template<typename TComponentList>
void ECSManager<TComponentList>::DestroyEntity (const Entity& entity) {
	assert(entity.uuid != 0 && "This is a deleted entity");
	assert(entities[entity.id].uuid == entity.uuid && "Entity uuid mismatch");
	entities[entity.id].uuid = 0;
	removedEntites.emplace_back(entity.id);
	EntityDestructor<TComponentList, 0>::DestroyComponent(this, entity);
}

#endif //RAPTOR_ECSMANAGER_HPP
