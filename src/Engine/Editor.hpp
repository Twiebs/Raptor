#pragma once

#include "World.hpp"
#include <Graphics/Render3D.hpp>

// TODO
/*
Camera movement 
	- move velocity
	- camera target
	- lerp between looking and stuff
Camera view modes
	- blender like interface for switching between view orientations 
*/

namespace Raptor {

struct Editor {
	std::string entityNames[MAX_ENTITIES];
	bool isSelected[64] = {};
	U32 selectedCount = 0;
	U32 lastSelected = 0;
	Camera camera;
};

void SaveEditor(const Editor& editor, const std::string& filename);
void LoadEditor(Editor& editor, const std::string& filename);
void ShowWorldEditor(World& world, Editor& editor);
}