#pragma once

#include "World.hpp"
#include <Graphics/Render3D.hpp>

namespace Raptor {

struct Editor {
	std::string entityNames[MAX_ENTITIES];
	S32 selected;
	Camera camera;
};

void SaveEditor(const Editor& editor, const std::string& filename);
void LoadEditor(Editor& editor, const std::string& filename);
void ShowWorldEditor(World& world, Editor& editor);
void ShowEntityInspector(World& world, Editor& editor);

}