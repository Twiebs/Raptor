#include "IEntityManager.h"


IEntityManager::IEntityManager() {
}


IEntityManager::~IEntityManager() {
}


std::unique_ptr<Entity>& IEntityManager::CreateEntity() {
	entities.push_back(std::make_unique<Entity>());
	return entities[entities.size() - 1];
}