#include "ISystem.hpp"



ISystem::ISystem(){
	
}


ISystem::~ISystem() {
}

void ISystem::AddedToManager(EntityManager* manager) {
	this->manager = manager;
}