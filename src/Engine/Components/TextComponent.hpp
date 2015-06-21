#pragma once

#include <Core/Component.hpp>
#include <Core/Common.hpp>
#include <Core/AssetManager.hpp>

#include <Assets/Font.hpp>
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
