#pragma once

#include <bitset>
#include <vector>
#include <Math/Vector3.hpp>
#include <memory>

#include <Core/Common.hpp>
#include <Core/Component.hpp>

#define MAX_SYSTEMS 64
#define MAX_COMPONENTS 64

//EntityManager foward decleration
class EntityManager;

class Entity {
public:
	//If an entity ID is zero there is a problem...
	EntityID GetID() { return id; }
	EntityUUID GetUUID() { return uuid; }

	Entity();
	Entity(EntityManager* manager, uint64 uuid, uint64 id);
	~Entity();

private:
	friend class EntityManager;
	//If the entity has an ID of zero then there is a serious problem
	EntityID id;
	EntityUUID uuid;

	std::bitset<MAX_COMPONENTS> componentBits;
	std::bitset<MAX_SYSTEMS> groupBits;
	EntityManager* manager;
};
