#pragma once
#include<Core\Engine.inc.h>

class IEntityManager {
public:
	IEntityManager();
	~IEntityManager();

	std::unique_ptr<Entity>& CreateEntity();

private:
	std::vector<std::unique_ptr<Entity>> entities;
};

