#pragma once

#include <Core\AssetManager.hpp>
#include <Core\Component.hpp>

struct ModelComponent : public Component {
	AssetID id;
};