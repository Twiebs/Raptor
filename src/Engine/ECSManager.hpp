#ifndef RAPTOR_ECSMANAGER_HPP
#define RAPTOR_ECSMANAGER_HPP
#include <Core/Common.hpp>
#include <cstring>
#include <functional>
#include <Utils/Metafuck.hpp>

template<typename T>
class StaticArray {
public:
	StaticArray(size_t count);
	~StaticArray();

	T& operator [](const size_t index) {
		assert(index < count && "StaticArray subscript out of bounds!");
		return elements[index];
	}

	size_t Count() {
		return count;
	}
private:
	T* elements;
	size_t count;
};



template<typename T>
class Array {
public:
	Array();
	Array(size_t initialCapacity);
	Array(size_t initialCapacity, const T& defaultValue);
	~Array();

	T& operator [](const size_t index) {
		assert(index <= count && "Array subscript out of bounds!");
		return data[index];
	}

	void Add(const T& element);

	template<typename... Args>
	void Create(Args... args) {
		if (count + 1 > capacity)
			Grow((capacity * 2) + 10);
		new (&data[count]) T(args...);
		count += 1;
	}

	template<typename... Args>
	T& CreateAndReturn(Args... args) {
		if (count + 1 > capacity)
			Grow((capacity * 2) + 10);
		T& element = data[count];
		new (&element) T(args...);
		count += 1;
		return element;
	}

	void Resize(size_t size);

	void Remove();

	T& PopAndReturn() {
		auto& element = data[count - 1];
		count--;
		return element;
	}

	size_t Count() {
		return count;
	}

private:
	T* data;
	size_t capacity;
	size_t count;

	void Grow(size_t newCapacity);
};


template<typename T>
Array<T>::Array() {
	data = nullptr;
	count = 0;
	capacity = 0;
}

template<typename T>
Array<T>::Array(size_t initialCapacity) {
	data = (T*)malloc(sizeof(T) * initialCapacity);
	count = 0;
	capacity = initialCapacity;
}

template<typename T>
Array<T>::Array(size_t initialCapacity, const T& defaultValue) {
	data = (T*)malloc(sizeof(T) * initialCapacity);
	count = initialCapacity;
	capacity = initialCapacity;
	for (U32 i = 0; i < count; i++) {
		data[i] = defaultValue;
	}
}

template<typename T>
Array<T>::~Array() {
	free(data);
}

template<typename T>
void Array<T>::Add (const T& element) {
	assert(data != nullptr);
	if (count + 1 > capacity) Grow(capacity * 2);
	data[count - 1] = element;
	count++;
}

template<typename T>
void Array<T>::Grow(size_t newCapacity) {
	void* newData = malloc(sizeof(T) * newCapacity);
	memcpy(newData, data, sizeof(T) * count);
	if (data != nullptr) free(data);
	capacity = newCapacity;
	data = (T*)newData;
}

template<typename T>
void Array<T>::Resize(size_t newSize) {
	assert(newSize > count && "Resize cannot shrink an array that has a larger count");
	void* newData = malloc(sizeof(T) * newSize);
	if (data != nullptr) {
		memcpy(newData, data, sizeof(T) * count);
		free(data);
		data = (T*)newData;
		capacity = newSize;
		count = newSize;
	} else {
		data = (T*)newData;
		capacity = newSize;
		count = newSize;
	}
}

template<typename TComponentList>
class ECSManager;

class ComponentManager {
public:
	ComponentManager(U32 componentTypeID, size_t componentSize, size_t initalSize);
	~ComponentManager();

	void* operator [](size_t index) {
		assert(index < count);
		return data + (componentSize * index);
	}

	void* CreateComponent(U32 entityID);
	S32 DestroyComponent(size_t id);

	size_t Count();
	size_t ComponentSize();
	void* Data();

private:
	template<typename TComponentList>
	friend class ECSManager;
	U32 componentTypeID;
	size_t componentSize;
	size_t count;
	size_t capacity;
	U8* data;

	Array<U32> owners;

	void Grow();
	void Grow(size_t newCapacity);
};

ComponentManager::ComponentManager (U32 componentTypeID, size_t componentSize, size_t initialCount)
	: componentTypeID(componentTypeID), componentSize(componentSize), count(0) {
	data = (U8*)malloc(componentSize * initialCount);
	owners.Resize(initialCount);
	capacity = initialCount;
}

ComponentManager::~ComponentManager() {
	free(data);
	count = 0;
	capacity = 0;
}

void* ComponentManager::CreateComponent (U32 entityID) {
	if (count + 1 > capacity)
		Grow();

	void* component = data + (componentSize * count);
	owners[count] = entityID;
	count += 1;
	return component;
}

S32 ComponentManager::DestroyComponent (size_t id) {
	U8* componentToDestroy = data + (componentSize * id);
	U8* componentToMove = data + (componentSize * (count - 1));
	memcpy(componentToDestroy, componentToMove, componentSize);

	auto newOwner = owners[count--];
	owners[id] = newOwner;
	return newOwner;
}

size_t ComponentManager::Count() {
	return count;
}

void* ComponentManager::Data() {
	return data;
}

size_t ComponentManager::ComponentSize() {
	return componentSize;
}

void ComponentManager::Grow() {
	Grow(capacity + 20); // This should have already been tuned to never grow
}

void ComponentManager::Grow (size_t newCapacity) {
	assert(data != nullptr);
	assert(componentSize > 0);
	U8* newData = (U8*)malloc(componentSize * newCapacity);
	memcpy(newData, data, componentSize * count);
	free(data);
	capacity = newCapacity;
	data = newData;
	owners.Resize(newCapacity);
}

struct Entity {
	U32 id;
	U64 uuid;
	Entity() { }
	Entity(U32 id, U64 uuid) : id(id), uuid(uuid) { }
};


template<typename TComponentList>
class ECSManager {
public:
	ECSManager(U32 initalEntityCount);
	~ECSManager();

	Entity& CreateEntity();
	void DestroyEntity(const Entity& entity);

	template <typename TComponent>
	TComponent* CreateComponent (const Entity &entity) {
		auto index = IndexOf<TComponent, TComponentList>();
		auto& manager = componentManagers[index];
		auto component = (TComponent*)manager.CreateComponent(entity.id);
		auto& assignedComponents = entityComponentTables[entity.id];
		assignedComponents[index] = manager.Count() - 1;
		return component;
	}

	template <typename TComponent>
	void DestroyComponent (const Entity& entity) {
		assert(entity.uuid != 0 && "This entity has been removed from the manager");
		auto componentTypeID = IndexOf<TComponent, TComponentList>();
		auto& components = entityComponentTables[entity.id];
		auto& manager = componentManagers[componentTypeID];
		assert(components[componentTypeID] != -1 && "Entity does not own this component type");
		auto newOwner = manager.DestroyComponent(components[componentTypeID]);

		auto& newOwnerComponents = entityComponentTables[newOwner];
		newOwnerComponents[componentTypeID] = newOwner;
	}

	template <typename TComponent>
	TComponent* GetComponent (const Entity& entity) {
		auto componentTypeIndex = IndexOf<TComponent, TComponentList>();
		auto& components = entityComponentTables[entity.id];
		auto& manager = componentManagers[componentTypeIndex];
		auto component = manager[components[componentTypeIndex]];
		return (TComponent*)component;
	}


	template<typename TComponent>
	void ProcessComponent (std::function<void(const Entity&, TComponent* component)> procedure) {
		auto componentTypeIndex = IndexOf<TComponent, TComponentList>();
		auto& manager = componentManagers[componentTypeIndex];
		TComponent* componentPointer = (TComponent*)manager.Data();
		for (U32 i = 0; i < manager.Count(); i++) {
			auto& entity = entities[manager.owners[i]];
			procedure(entity, componentPointer);
			componentPointer += 1;
		}
	}


	private:
	friend class ComponentManager;

	Array<Entity> entities;
	Array<U32> removedEntityIDs;
	Array<Array<S32>> entityComponentTables;

	Array<ComponentManager> componentManagers;

	U32 nextUUID = 1;
};

template <typename TComponentList>
ECSManager<TComponentList>::ECSManager(U32 initalEntityCount) {
	entities.Resize(initalEntityCount);
	entityComponentTables.Resize(initalEntityCount);
	auto componentCount = TComponentList::Size();
	componentManagers.Resize(TComponentList::Size());
//	for (U32 i = 0; i < componentCount; i++) {
//		componentManagers.Create(i, sizeof(AtIndex<i, TComponentList>), size_t initalSize)
//	}
}
template<typename TComponentList>
ECSManager<TComponentList>::~ECSManager() {

}

template<typename TComponentList>
Entity& ECSManager<TComponentList>::CreateEntity() {
	if (removedEntityIDs.Count() > 0) {
		auto id = removedEntityIDs.PopAndReturn();
		auto& entity = entities[id];
		entity.uuid = nextUUID++;
		return entity;
	}

	auto& entity = entities.CreateAndReturn(entities.Count(), nextUUID++);
	entityComponentTables.CreateAndReturn(TComponentList::Size(), -1);
	return entity;
}

template<typename TComponentList>
void ECSManager<TComponentList>::DestroyEntity (const Entity& entity) {
	assert(entity.uuid != 0 && "This is a deleted entity");
	assert(entities[entity.id].uuid == entity.uuid && "Entity uuid mismatch");
	entities[entity.id].uuid = 0;
	removedEntityIDs.Add(entity.id);

	auto& assignedComponents = entityComponentTables[entity.id];
	for (U32 i = 0; i < assignedComponents.Count(); i++) {
		if (assignedComponents[i] != -1) {
			auto& allocator = componentManagers[i];
			auto newOwner = allocator.DestroyComponent(assignedComponents[i]);
			auto& newOwnerComponents = entityComponentTables[newOwner];
			newOwnerComponents[i] = newOwner;
		}
	}
}
//
//template<typename TComponentList, typename TComponentType>
//void* ECSManager<TComponentList>::CreateComponent<TComponentType> (const Entity &entity) {
//	assert(componentManagers.Count() > IndexOf<TComponentType, TComponentList>() && "componentTypeIndex out of bounds!");
//	auto index = IndexOf<TComponentType, TComponentList>();
//	auto& manager = componentManagers[index];
//	auto component = manager.CreateComponent(entity.id);
//	auto& assignedComponents = entityComponentTables[entity.id];
//	assignedComponents[index] = manager.Count() - 1;
//	return component;
//}

//template <typename TComponentList, typename TComponentType>
//void ECSManager<TComponentList>::DestroyComponent<TComponentType> (const Entity& entity) {
//	assert(entity.uuid != 0 && "This entity has been removed from the manager");
//	auto componentTypeID = TypeOf<TComponentType, TComponentList>();
//	auto& components = entityComponentTables[entity.id];
//	auto& manager = componentManagers[componentTypeID];
//	assert(components[componentTypeID] != -1 && "Entity does not own this component type");
//	auto newOwner = manager.DestroyComponent(components[componentTypeID]);
//
//	auto& newOwnerComponents = entityComponentTables[newOwner];
//	newOwnerComponents[componentTypeID] = newOwner;
//}

//void ECSManager::ProcessComponent(U32 componentTypeID, std::function<void(const Entity&, void*)> procedure) {
//	assert(componentTypeID < componentTypesRegistered);
//	auto& manager = componentManagers[componentTypeID];
//	U8* componentPointer = (U8*)manager.Data();
//	for (U32 i = 0; i < manager.Count(); i++) {
//		auto& entity = entities[manager.owners[i]];
//		procedure(entity, componentPointer);
//		componentPointer += manager.ComponentSize();
//	}
//}

//
//template <typename TComponentList, typename TComponent>
//void ECSManager<TComponentList>::ProcessComponent<TComponent>(std::function<void(const Entity&, TComponent* component)> procedure) {
//	auto componetTypeID = IndexOf<TComponent, TComponentList>();
//	auto& manager = componentManagers[componentTypeID];
//	TComponent* componentPointer = manager.Data();
//	for (U32 i = 0; i < manager.Count(); i++) {
//		auto& entity = entities[manager.owners[i]];
//		procedure(entity, componentPointer);
//		componentPointer += 1;
//	}
//}



//void* ECSManager::GetComponent(const Entity& entity, U32 componentTypeID) {
//	auto& components = entityComponentTables[entity.id];
//	auto& manager = componentManagers[componentTypeID];
//	auto component = manager[components[componentTypeID]];
//	return component;
//}

#endif //RAPTOR_ECSMANAGER_HPP
