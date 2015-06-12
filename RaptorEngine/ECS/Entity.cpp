
#include <ECS\Entity.hpp>

Entity::Entity() {
}

Entity::Entity(EntityManager* manager, uint64 uuid, uint64 id) : 
	manager(manager), uuid(uuid), id(id) { 
}

Entity::~Entity() {

}
