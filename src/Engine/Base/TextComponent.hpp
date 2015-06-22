#pragma once

#include <Core/Common.hpp>
#include <ECS/Component.hpp>

#include <Graphics/Color.hpp>

struct TextComponent : public Component {
	uint32 x, y;
	uint16 width, height;
	Color color;
	std::string string;
	AssetID fontID;

	TextComponent() { }
	TextComponent(const char* string, AssetID fontID) : string(string), fontID(fontID) { }
};
