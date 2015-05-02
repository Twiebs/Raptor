#pragma once

#include<Core\Component.h>

#include<vector>
#include<Math\Vector3.h>
#include<memory>

class Entity {
public:
	unsigned int tag;
	unsigned int flags;

	Vector3 position;
	Vector3 rotation;
	Vector3 scale;

	Entity();
	~Entity();

	void AddComponent(std::unique_ptr<Component> component);
	void RemoveComponent(Component* component); //FIXME: bad... wont do a damn thing
	std::unique_ptr<Component>& GetComponent();

private:
	std::vector<std::unique_ptr<Component>> components;
};

