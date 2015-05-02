#include "Entity.h"


Entity::Entity() {
}


Entity::~Entity() {
}

void Entity::AddComponent(std::unique_ptr<Component> component) {
	components.push_back(std::move(component));
}

//This is nonsense dont use it!
void Entity::RemoveComponent(Component* component) {

}

std::unique_ptr<Component>& Entity::GetComponent() {
	return components[0];
}