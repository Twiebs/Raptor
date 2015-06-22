#include "Entity.hpp"

Entity::Entity() {
}

Entity::Entity(ECSManager* manager, uint64 uuid, uint64 id) :
	manager(manager), uuid(uuid), id(id) {
}

Entity::~Entity() {

}
