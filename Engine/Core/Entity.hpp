#pragma once

#include<Core\Common.hpp>

#include<bitset>
#include<vector>
#include<Math\Vector3.hpp>
#include<memory>

#include<Core\Component.hpp>

#define MAX_SYSTEMS 64
#define MAX_COMPONENTS 64

class EntityManager;

typedef uint64 EntityID;
typedef uint64 EntityUUID;

class Entity {
public:
	//If an entity ID is zero there is a problem...
	EntityID GetID() { return id; }
	EntityUUID GetUUID() { return uuid; }

	Entity();
	Entity(EntityManager* manager, uint64 uuid, uint64 id);
	~Entity();

	//Creates a component and adds it to the pool in the entity manager... not yet
	template<typename T, typename... Args>
	T* AddComponent(Args... args) {
		T* component = manager->CreateComponent<T>(id, args...);
		return component;
	}

	template<typename T>
	void RemoveComponent() {
		manager->RemoveComponent<T>();
	}

	template<typename T>
	std::unique_ptr<T>& GetComponent() {

	}

private:
	friend class EntityManager;
	EntityID id;
	EntityUUID uuid;

	std::bitset<MAX_COMPONENTS> componentBits;
	std::bitset<MAX_SYSTEMS> groupBits;
	EntityManager* manager;
};

