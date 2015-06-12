#pragma once

#include<Core\Types.h>

#include<bitset>
#include<vector>
#include<Math\Vector3.hpp>
#include<memory>

#include<ECS\Component.hpp>

#define MAX_SYSTEMS 64
#define MAX_COMPONENTS 64

class EntityManager;


class Entity {
public:
	//If an entity ID is zero there is a problem...
	uint64 GetID() { return id; }
	uint64 GetUUID() { return uuid; }

	Entity();
	Entity(EntityManager* manager, uint64 uuid, uint64 id);
	~Entity();

	//Creates a component and adds it to the pool in the entity manager... not yet
	template<typename T, typename... Args>
	T* AddComponent(Args... args) {

		T* component = manager->CreateComponent<T>(this, args...);
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
	uint64 id;
	uint64 uuid;
	std::bitset<MAX_COMPONENTS> componentBits;
	std::bitset<MAX_SYSTEMS> groupBits;
	EntityManager* manager;
};

