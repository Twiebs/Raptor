#include "IService.hpp"

void IService::AddedToEngine(Engine* engine) {
	this->engine = engine;
}