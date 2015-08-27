#pragma once

#include <Core/Common.hpp>
#include <ECS/Component.hpp>

struct ModelComponent : public Component {
	AssetID id;
};
