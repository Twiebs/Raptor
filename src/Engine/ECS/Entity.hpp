#pragma once

#include <Core/Common.hpp>

//struct Entity {
//	uint32 id = 0;
//	uint64 uuid = 0;
//};

class ECSManager;
class Entity {
	friend class ECSManager;
public:
	//If an entity ID is zero there is a problem...
	inline EntityID GetID() {
		return id;
	}
	inline EntityUUID GetUUID() {
		return uuid;
	}

	Entity(ECSManager* manager, uint64 uuid, uint64 id) :
			manager(manager), uuid(uuid), id(id) {
	}

	Entity() { }
	~Entity() { }

private:
	EntityID id = 0;
	EntityUUID uuid = 0;
	ECSManager* manager = nullptr;
};
