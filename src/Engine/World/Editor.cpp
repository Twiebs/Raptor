#include "Editor.hpp"

#include <Graphics/GUI.hpp>
#include <Core/Platform.h>

namespace Raptor {

void ShowWorldEditor(World& world, Editor& editor) {
	ImGui::Begin("World Editor");
	ImGui::BeginChild("Entities", ImVec2(ImGui::GetWindowWidth(), 300));
	for (auto i = 0; i < world.entityCount; i++) {
		ImGui::PushID(i);
		if (ImGui::Button(editor.entityNames[i].c_str())) {
			if (PlatformGetKey(KEY_LCTRL)) {
				auto& transform = (Transform3D&)world.components[COMPONENT_TRANSFORM][i];
				editor.camera.position = transform.translation + Vector3(0.0f, 1.0f, 6.0f);
				editor.camera.pitch = 0;
				editor.camera.yaw = -90;
			}
			editor.selected = i;
		}
		ImGui::PopID();
	}

	ImGui::EndChild();
	ImGui::Separator();
	if (ImGui::Button("Save World")) {
		SaveWorld(world, "test.world");
		SaveEditor(editor, "test.editor");
	}
	ImGui::End();
}

void ShowEntityInspector(World& world, Editor& editor) {
	ImGui::Begin("Entity Inspector");
	static char entityLabel[64];
	ImGui::InputText("Label: ", entityLabel, 64, ImGuiInputTextFlags_EnterReturnsTrue);
	ImGui::SameLine();
	if (ImGui::Button("Apply")) {
		editor.entityNames[editor.selected] = entityLabel;
	}
	ImGui::Text("Transform");
	Transform3D* transforms = (Transform3D*)world.components[COMPONENT_TRANSFORM];
	auto& transform = transforms[editor.selected];
	ImGui::DragFloat3("Position", &transform.translation.x, 0.2f);
	ImGui::DragFloat3("Rotation", &transform.rotation.x, 0.2f);
	ImGui::DragFloat3("Scale", &transform.scale.x, 0.2f);
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