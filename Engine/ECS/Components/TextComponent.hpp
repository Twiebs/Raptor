#pragma once

#include <UI\Font.h>
#include <ECS\Component.hpp>
#include <Core\Types.h>
#include <Graphics\Color.h>
#include <Core\AssetManager.hpp>

struct TextComponent : public Component {
	uint32 x, y;
	Color color;
	std::string text;
	Font* font;
	AssetID fontID;
};