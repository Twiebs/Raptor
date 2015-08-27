#include "Editor.hpp"

#include <Graphics/GUI.hpp>
#include <Core/Platform.h>

namespace Raptor {

void ShowWorldEditor(World& world, Editor& editor) {
	static char entityLabel[64];
	ImGui::Begin("Entity Explorer", nullptr, ImGuiWindowFlags_MenuBar);

	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("Menu")) {
			
			if (ImGui::MenuItem("Load World")) {
				LoadWorld(world, "test.world");
				LoadEditor(editor, "test.editor");
			}

			if (ImGui::MenuItem("Save World")) {
				SaveWorld(world, "test.world");
				SaveEditor(editor, "test.editor");
			}

			ImGui::EndMenu();
		}
	}

	ImGui::EndMenuBar();

	ImGui::BeginChild("EntityList", ImVec2(200, 0), true);
	for (auto i = 0; i < world.entityCount; i++) {
		if (ImGui::Selectable(editor.entityNames[i].c_str(), editor.isSelected[i])) {
			// Snap to target
			if (PlatformGetKey(KEY_LALT)) {
				auto transforms = (Transform3D*)world.components[COMPONENT_TRANSFORM];
				auto& transform = transforms[i];
				editor.camera.position = transform.translation + Vector3(0.0f, 1.0f, 6.0f);
				editor.camera.pitch = 0;
				editor.camera.yaw = -90;
			}

			// Entity Selection
			else if (editor.selectedCount >= 1) {
				if (PlatformGetKey(KEY_LSHIFT)) {
					int diff = i - editor.lastSelected;
					bool* ptr = &editor.isSelected[(diff > 0) ? editor.lastSelected + 1 : i];
					memset(ptr, true, abs(diff));
					editor.lastSelected = i;
					editor.selectedCount += abs(diff);
				} else if (PlatformGetKey(KEY_LCTRL)) {
					editor.isSelected[i] = !editor.isSelected[i];
					if (editor.isSelected[i]) {
						editor.selectedCount++;
						editor.lastSelected = i;
					} else {
						editor.selectedCount--;
					}
				} else {
					memset(editor.isSelected, 0, 64);
					editor.isSelected[i] = true;
					editor.lastSelected = i;
					editor.selectedCount = 1;
				}
			}

			else {
				editor.isSelected[i] = !editor.isSelected[i];
				if (editor.isSelected[i]) {
					editor.selectedCount++;
					editor.lastSelected = i;
				} else {
					editor.selectedCount--;
				}
			}
			sprintf(entityLabel, editor.entityNames[i].c_str());
		}
	}
	ImGui::EndChild();
	ImGui::SameLine();

	ImGui::BeginGroup();
	if (editor.selectedCount == 1) {
		ImGui::InputText("Label: ", entityLabel, 64, ImGuiInputTextFlags_EnterReturnsTrue);
		ImGui::SameLine();
		if (ImGui::Button("Apply")) editor.entityNames[editor.lastSelected] = entityLabel;
		ImGui::Separator();

		ImGui::Text("Transform");
		Transform3D* transforms = (Transform3D*)world.components[COMPONENT_TRANSFORM];
		auto& transform = transforms[editor.lastSelected];
		ImGui::DragFloat3("Position", &transform.translation.x, 0.2f);
		ImGui::DragFloat3("Rotation", &transform.rotation.x, 0.2f);
		ImGui::DragFloat3("Scale", &transform.scale.x, 0.2f);
	}		
	ImGui::EndGroup();

	ImGui::End();
}

void SaveEditor(const Editor& editor, const std::string& filename) {
	U32 entityCount = 64;
	SDL_RWops* file = SDL_RWFromFile(filename.c_str(), "w");
	if (file == nullptr) {
		LOG_ERROR("Could not open " << filename << " when saving editor");
		return;
	}

	SDL_RWwrite(file, &entityCount, sizeof(U32), 1);
	SDL_RWseek(file, sizeof(U32), RW_SEEK_CUR);
	for (auto i = 0; i < entityCount; i++) {
		U8 size = editor.entityNames[i].length();
		SDL_RWwrite(file, &size, 1, 1);
		SDL_RWseek(file, 1, RW_SEEK_CUR);
		SDL_RWwrite(file, editor.entityNames[i].c_str(), 1, size);
		SDL_RWseek(file, size, RW_SEEK_CUR);
	}

	SDL_RWclose(file);
}

void LoadEditor(Editor& editor, const std::string& filename) {
	SDL_RWops* file = SDL_RWFromFile(filename.c_str(), "r");
	if (file == nullptr) {
		LOG_ERROR("Could not open " << filename << " when loading editor");
		return;
	}
	U32 entityCount;
	SDL_RWread(file, &entityCount, sizeof(U32), 1);
	SDL_RWseek(file, sizeof(U32), RW_SEEK_CUR);
	char length, temp_buffer[256];
	for (auto i = 0; i < entityCount; i++) {
		memset(temp_buffer, 0, 256);
		SDL_RWread(file, &length, 1, 1);
		SDL_RWseek(file, 1, RW_SEEK_CUR);
		SDL_RWread(file, temp_buffer, 1, length);
		SDL_RWseek(file, length, RW_SEEK_CUR);
		editor.entityNames[i] = temp_buffer;
	}

	SDL_RWclose(file);
}

}