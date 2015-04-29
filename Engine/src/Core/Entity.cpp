#include "Entity.h"


Entity::Entity() {
}


Entity::~Entity() {
}

void Entity::AddComponent(Component* component) {
	components.push_back(component);
}

void Entity::RemoveComponent(Component* component) {
	//This is nonsense dont use it!
}

Component* Entity::GetComponent() {
	return components[0];
}