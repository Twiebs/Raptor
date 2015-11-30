#include "Editor.hpp"

#include <Core/logging.h>
#include <Engine/Engine.hpp>

#include <Graphics/imgui.h>
#include <Core/Platform.h>
#include <Utils/global_profiler.h>

#include <string>

static void ShowAssetBrowser (AssetBrowser* assetBrowser);

template<typename TArchive>
void serialize(TArchive& archive, FileWriteTime& writeTime) {
	archive(writeTime.lowDateTime);
	archive(writeTime.highDateTime);
}

template<typename TArchive>
void serialize(TArchive& archive, EditorOfflineState& state) {
//	archive(state.lastAssetDirectoryWriteTime);
}

EditorOfflineState::EditorOfflineState() {
	std::ifstream stream("editor_offline_state");
	if (!stream.is_open()) return;

	cereal::JSONInputArchive archive(stream);
	archive(*this);
}

EditorOfflineState::~EditorOfflineState() {
	std::ofstream stream("editor_offline_state");
	cereal::JSONOutputArchive archive(stream);
	archive(*this);
}

void CheckAssetFolderForChanges(EditorWindow* editor) {
	//auto lastWriteTime = GetLastWriteTimeForDirectory(ASSET_DIRECTORY);
	//if (editor->state.lastAssetDirectoryWriteTime != lastWriteTime) {
	//	std::cout << "The Asset Directory was Modifed!\n";
	//	editor->state.lastAssetDirectoryWriteTime = lastWriteTime;
	//}
}

void ImportAsset(const std::string& filename, AssetManifest* manifest) {
	auto extentionBegin = filename.find_last_of('.') + 1;
	auto folderNameEnd = filename.find_first_of('/');
	auto fileExtention = filename.substr(extentionBegin, filename.size() - extentionBegin);
	auto assetName = filename.substr(0, folderNameEnd);
	if (fileExtention == "blend") {
		bool isAssetNameTaken = manifest->nameToEntryIndexMap[assetName] > 0;
		if (isAssetNameTaken) {
			assert(false && "An asset with this name has already been added to the manifest!");
		} else {
			manifest->AddEntry(assetName, filename);
		}
	} else {
		assert(false && "Unsupported asset type!");
	}
}



HotReloadWatchList::HotReloadWatchList() {
	internalPollIterator = filenameEntryMap.begin();
}

HotReloadWatchList::~HotReloadWatchList() { }

void HotReloadWatchList::AddWatch (const std::string& filename, U32 flags, U32 entryID) {

	auto lastWriteTime = GetLastModifedTime(filename);
	HotReloadEntry entry;
	entry.filename = filename;
	entry.time = lastWriteTime;
	entry.flags = flags;
	entry.id = entryID;

	filenameEntryMap[filename] = entry;

	internalPollIterator = filenameEntryMap.begin();
	//entries.emplace_back(entry);
	//S32 index = entries.size() - 1;
	//filenameIndexMap[filename] = index;
}

void HotReloadWatchList::RemoveWatch (const std::string& filename) {
	filenameEntryMap.erase(filename);
	internalPollIterator = filenameEntryMap.begin();
	/*S32 index = filenameIndexMap[filename];
	entries.erase(entries.begin() + index);
	filenameIndexMap[filename] = -1;*/
}


U32 HotReloadWatchList::PollHotreloadState (HotReloadEntry** out_entry) { 
	if (internalPollIterator == filenameEntryMap.end()) {
		internalPollIterator = filenameEntryMap.begin();
		return 0;
	}

	auto& entry = internalPollIterator->second;
	internalPollIterator++;
	auto lastWriteTime = GetLastModifedTime(entry.filename);
	if (lastWriteTime != entry.time) {
		entry.time = lastWriteTime;
		*out_entry = &entry;
		return 1;
	}

	*out_entry = nullptr;
	return 1;

	//if (internalPollIndex >= entries.size()) {
	//	internalPollIndex = 0;
	//	return 0;
	//}
	//
	//auto& entry = entries[internalPollIndex++];
	//auto lastWriteTime = GetLastModifedTime(entry.filename);
	//if (lastWriteTime != entry.time) {
	//	entry.time = lastWriteTime;
	//	*out_entry = &entry;
	//	return 1;
	//}

	//*out_entry = nullptr;
	//return 1;
}

#include <Engine/GlobalAssetManager.hpp>

static void HotreloadModifedAssets (HotReloadWatchList* watchList) {
	HotReloadEntry* entry;
	U32 pollNotFinished;
	while (pollNotFinished = watchList->PollHotreloadState(&entry)) {
		if (entry == nullptr) break;	// This entry has not been modifed!

		switch (entry->flags) {
		case ASSET_SHADER:
			ReloadShader(entry->id);
			break;
		}
	}
}

static void ShowAssetManifestBrowser(AssetBrowser* assetBrowser);


void ShowEditorWindow (EditorWindow* editor) {
	HotreloadModifedAssets(&editor->assetBrowser.hotreloadWatchList);

	if (editor->selectedWindowIndex == -1) return;
	ImGui::Begin("EditorWindow", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar);
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::BeginMenuBar();
	if (ImGui::Button("Asset Browser")) editor->selectedWindowIndex = 0;
	ImGui::SameLine();
	if (ImGui::Button("Profiler")) editor->selectedWindowIndex = 1;
	ImGui::SameLine(); 
	if (ImGui::Button("Render Settings")) editor->selectedWindowIndex = 2;
	ImGui::EndMenuBar();
	ImGui::PopStyleColor();
	
	switch (editor->selectedWindowIndex) {
	case 0: ShowAssetManifestBrowser(&editor->assetBrowser); break;
	case 1: ShowProfilerPanel(GetGlobalProfiler()); break;
	case 2: ShowDebugRenderSettings(&editor->renderSettings); break;
	}

	ImGui::End();
}

void ProcessEditorKeypress (EditorWindow* editor, int keycode) {
	static auto ToggleIndex = [](S32* lastIndex, S32 newIndex) {
		if (*lastIndex == newIndex) *lastIndex = -1;
		else *lastIndex = newIndex;
	};

	if (keycode == KEY_ESCAPE) editor->selectedWindowIndex = -1;
	else if (keycode == KEY_F1) ToggleIndex(&editor->selectedWindowIndex, 0);
	else if (keycode == KEY_F2) ToggleIndex(&editor->selectedWindowIndex, 1);
	else if (keycode == KEY_F3) ToggleIndex(&editor->selectedWindowIndex, 2);
}



void ShowProfilerPanel (Profiler* profiler) {
	ImGui::BeginChild("PersistantEntries", ImVec2(400.0f, 0.0f), true);
	for (U32 i = 0; i < profiler->activePersistantEntries.size(); i++) {
		auto entryIndex = profiler->activePersistantEntries[i];
		auto entry = &profiler->persistantEntries[entryIndex];
		if (ImGui::CollapsingHeader(entry->name.c_str())) {
			auto timeIndex = entry->writeIndex - 1;
			if (timeIndex < 0) timeIndex = PROFILER_PERSISTANT_SAMPLE_COUNT;
			ImGui::Text("Elapsed Time: %f ms", entry->elapsedTimes[timeIndex]);
			ImGui::Text("Elapsed Cycles: %d", entry->elapsedCycles[timeIndex]);
			ImGui::PushID(i);
			ImGui::PlotLines("", entry->elapsedTimes, PROFILER_PERSISTANT_SAMPLE_COUNT, timeIndex);
			ImGui::PopID();
		}
	}
	ImGui::EndChild();
	ImGui::SameLine();
	ImGui::BeginChild("InstantaneousEntries", ImVec2(250.0f, 0.0f), true);
	for (U32 i = 0; i < profiler->newEntryIndex; i++) {
		auto& entry = profiler->entires[i];
		ImGui::BeginChildFrame(i, ImVec2(235.0f, 50.0f));
		ImGui::Text(entry.name.c_str());
		ImGui::Text("Elapsed Time: %f ms", entry.elapsedTime);
		ImGui::Text("Elapsed Cycles: %d", entry.elapsedCycles);
		ImGui::EndChildFrame();
	}

	ImGui::EndChild();
}


// If the entry index is set to -1 this assumes that a new asset manifest entry is being created
// Will return 1 if an entry was modifed or added, -1 if canceled, 0 otherwise,  This is currently useless
// @MINOR this probably doesnt need to return a value
static inline int ShowAssetManifestEntryEdit(AssetManifest* manifest, S64 entryIndex, bool* setInputBuffersToEntryValues) {
	static const int NAME_INPUT_BUFFER_SIZE = 256;
	static const int FILENAME_INPUT_BUFFER_SIZE = 1024;
	static char nameInputBuffer[NAME_INPUT_BUFFER_SIZE];
	static char filenameInputBuffer[FILENAME_INPUT_BUFFER_SIZE];
	//static std::string nameInputBuffer(NAME_INPUT_BUFFER_SIZE, 0);
	//static std::string filenameInputBuffer(FILENAME_INPUT_BUFFER_SIZE, 0);

	// Takes a pointer to the bool and sets it here to force it to be handled in this function
	if (*setInputBuffersToEntryValues) {
		assert(entryIndex >= 0);
		auto& entry = manifest->entries[entryIndex];
		//nameInputBuffer = entry.name;
		//filenameInputBuffer = entry.filename;
		assert((entry.name.size() + 1) < NAME_INPUT_BUFFER_SIZE);
		assert((entry.filename.size() + 1) < FILENAME_INPUT_BUFFER_SIZE);
		memcpy(nameInputBuffer, &entry.name.front(), entry.name.size() + 1);
		memcpy(filenameInputBuffer, &entry.filename.front(), entry.filename.size() + 1);
		*setInputBuffersToEntryValues = false;
	}

	ImGui::InputText("AssetName", nameInputBuffer, NAME_INPUT_BUFFER_SIZE);
	ImGui::InputText("Filename", filenameInputBuffer, FILENAME_INPUT_BUFFER_SIZE);
	//ImGui::InputText("AssetName", &nameInputBuffer.front(), nameInputBuffer.capacity());
	//ImGui::InputText("Filename", &filenameInputBuffer.front(), filenameInputBuffer.capacity());

	if (entryIndex == -1) {
		if (ImGui::Button("Add Entry")) {
			manifest->AddEntry(nameInputBuffer, filenameInputBuffer);
			return 1;
		}
	} else {
		if (ImGui::Button("Apply Changes")) {
			auto entry = &manifest->entries[entryIndex];
			entry->name = nameInputBuffer;
			entry->filename = filenameInputBuffer;
			return 1;
		}
	}

	if (ImGui::Button("Cancel")) {
		return -1;
	}

	return 0;
}


static void ShowAssetManifestBrowser(AssetBrowser* assetBrowser) {
	auto assetManifest = assetBrowser->manifest;

	ImGui::Text("Manifest Entry Count: %d", assetManifest->entries.size());
	ImGui::Text("Loaded Asset Count: %d", assetManifest->loadedCount);
	ImGui::SameLine();
	if (ImGui::Button("Save Manifest File")) {
		assetManifest->Serialize();
	}


	static auto AssetListTableHeader = [](const char* assetListName) {
		ImGui::Separator();
		ImGui::Columns(4, assetListName);
		ImGui::Separator();
		ImGui::Text("Name");
		ImGui::NextColumn();
		ImGui::Text("State");
		ImGui::NextColumn();
		ImGui::Text("Hotreload");
		ImGui::NextColumn();
		ImGui::Text("Filename");
		ImGui::NextColumn();
		ImGui::Separator();
	};

	static bool setManifestEntryEditorBuffers = false;
	auto ShowAssetManifestEntries = [&]() {
		AssetListTableHeader("AssetManifest");

		for (U32 i = 0; i < assetManifest->entries.size(); i++) {
			auto& entry = assetManifest->entries[i];
			if (ImGui::Selectable(entry.name.c_str(), assetBrowser->selectedAssetIndex == i, ImGuiSelectableFlags_SpanAllColumns)) {
				assetBrowser->selectedAssetIndex = i;
				setManifestEntryEditorBuffers = true;
			}

			ImGui::NextColumn();
			auto& assetState = assetManifest->assetStates[i];
			auto textColor = assetState.loadedState == AssetLoadState::UNLOADED ? ImVec4(1.0, 0.0, 0.0, 1.0) : ImVec4(0.0, 1.0, 0.0, 1.0);
			auto textValue = assetState.loadedState == AssetLoadState::UNLOADED ? "UNLOADED" : "LOADED";
			ImGui::TextColored(textColor, textValue);
			ImGui::NextColumn();
			ImGui::Text("NO");
			ImGui::NextColumn();
			ImGui::Text(entry.filename.c_str());
			ImGui::NextColumn();
		}
	};


	ImGui::Columns(2, "MainAssetManifestPartition");
	ImGui::BeginChild("EntryList");
	ShowAssetManifestEntries();
	ImGui::EndChild();
	ImGui::NextColumn();
	ImGui::Separator();
	ImGui::BeginChild("EntryEdit");
	ShowAssetManifestEntryEdit(assetManifest, assetBrowser->selectedAssetIndex, &setManifestEntryEditorBuffers);
	ImGui::EndChild();



}

static void ShowAssetBrowser (AssetBrowser* assetBrowser) {
	if (ImGui::Button("Materials")) {
		assetBrowser->selectedAssetType = ASSET_MATERIAL;
		assetBrowser->selectedAssetIndex = -1;
	}

	ImGui::SameLine();
	if (ImGui::Button("Shaders")) {
		assetBrowser->selectedAssetType = ASSET_SHADER;
		assetBrowser->selectedAssetIndex = -1;
	}

	ImGui::SameLine();
	if (ImGui::Button("Models")) {
		assetBrowser->selectedAssetType = ASSET_MODEL;
		assetBrowser->selectedAssetIndex = -1;
	}

	auto assetManager = GetGlobalAssetManager();
	ImGui::BeginChild("AssetList", ImVec2(450.0f, 0.0f), true);
	switch (assetBrowser->selectedAssetType) {
	case 0:
		for (U32 i = 0; i < assetManager->materials.size(); i++) {
			const auto& info = assetManager->materialAssetInfos[i];
			if (ImGui::Button(info.name.c_str(), ImVec2(200.0f, 0.0f))) {
				assetBrowser->selectedAssetIndex = i;
			}
		}
		break;

	case 1:
		ImGui::Columns(2, "ShaderList");
		ImGui::Separator();
		ImGui::Text("Name");
		ImGui::NextColumn();
		ImGui::Text("Hotreload");
		ImGui::NextColumn();
		ImGui::Separator();

		for (U32 i = 0; i < assetManager->shaderPrograms.size(); i++) {
			const auto& data = assetManager->shaderBuilderData[i];
			if (ImGui::Selectable(data.name.c_str(), assetBrowser->selectedAssetIndex == i, ImGuiSelectableFlags_SpanAllColumns)) {
				assetBrowser->selectedAssetIndex = i;
			}

			ImGui::NextColumn();
			ImGui::Text(data.hotreloadEnabled ? "YES" : "NO");
			ImGui::NextColumn();
		}



		//for (U32 i = 0; i < manifest.shaderPrograms.size(); i++) {
		//	const auto& data = manifest.shaderBuilderData[i];
		//	if (ImGui::Button(data.name.c_str(), ImVec2(200.0f, 0.0f))) {
		//		assetBrowser->selectedAssetIndex = i;
		//	}
		//}

		break;

	case 2:
		for (U32 i = 0; i < assetManager->models.size(); i++) {
			auto& info = assetManager->modelInfo[i];
			if (ImGui::Button(info.name.c_str(), ImVec2(200.0f, 0.0f))) {
				assetBrowser->selectedAssetIndex = i;
			}
		}
		break;
	}
	


	ImGui::EndChild();

	ImGui::SameLine();
	ImGui::BeginChild("AssetInspector", ImVec2(0, 0), true);
	if (assetBrowser->selectedAssetType == 1) {
		ImGui::BeginChild("All Shaders", ImVec2(0, 50), true);
		if (ImGui::Button("Reload All Shader Programs!")) {
			for (U32 i = 0; i < assetManager->shaderPrograms.size(); i++) {
				ReloadShader(ShaderHandle { i });
			}
		}
		ImGui::EndChild();
	}



	if (assetBrowser->selectedAssetIndex >= 0) {
		if (assetBrowser->selectedAssetType == 0) {
			auto& material = assetManager->materials[assetBrowser->selectedAssetIndex];
			static auto ShowMaterialTexture = [](GLuint textureID, MaterialParameter paramType) {
				ImGui::BeginChild(textureID, ImVec2(130.0f, 158.0f), true);
				ImGui::Text(GetMaterialParameterString(paramType));
				ImGui::Image((ImTextureID)textureID, ImVec2(128.0f, 128.0f));
				ImGui::EndChild();
			};

			ShowMaterialTexture(material.diffuseMapID, MATERIAL_DIFFUSE);
			ImGui::SameLine();
			ShowMaterialTexture(material.specularMapID, MATERIAL_SPECULAR);
			ImGui::SameLine();
			ShowMaterialTexture(material.normalMapID, MATERIAL_NORMAL);

			if (ImGui::Button("Reload Material")) {
				LOG_WARNING("Material reloading not currently implemented!");
			}

		} else if (assetBrowser->selectedAssetType == 1) {
			ShaderBuilderData* shaderData = (ShaderBuilderData*)&assetManager->shaderBuilderData[assetBrowser->selectedAssetIndex];
			if (ImGui::Checkbox("Hotreload Enabled", &shaderData->hotreloadEnabled)) {
				if (shaderData->hotreloadEnabled) {
					auto AddWatchIfShaderHasType = [&](ShaderType type) {
						auto typeFlag = GetShaderTypeFlag(type);
						if (shaderData->flags & typeFlag) {
							assetBrowser->hotreloadWatchList.AddWatch(shaderData->sourceFilenames[type], ASSET_SHADER, assetBrowser->selectedAssetIndex);
						}
					};


					AddWatchIfShaderHasType(VERTEX_SHADER);
					AddWatchIfShaderHasType(FRAGMENT_SHADER);
					AddWatchIfShaderHasType(GEOMETRY_SHADER);
					AddWatchIfShaderHasType(COMPUTE_SHADER);

				} else {
					auto RemoveWatchIfShaderHasType = [&](ShaderType type) {
						if (shaderData->flags & GetShaderTypeFlag(type)) {
							assetBrowser->hotreloadWatchList.RemoveWatch(shaderData->sourceFilenames[type]);
						}
					};

					RemoveWatchIfShaderHasType(VERTEX_SHADER);
					RemoveWatchIfShaderHasType(FRAGMENT_SHADER);
					RemoveWatchIfShaderHasType(GEOMETRY_SHADER);
					RemoveWatchIfShaderHasType(COMPUTE_SHADER);
				}
			}

			if (ImGui::Button("Reload Shader Now")) {
				assetManager->ReloadShader(ShaderHandle { (U32)assetBrowser->selectedAssetIndex });
			}
		}
	}

	else {


	}


	ImGui::EndChild();
}

void ShowDebugRenderSettings (DebugRenderSettings* settings) {
	ImGui::Checkbox("Disable Normal Mapping", &settings->disableNormalMaps);
	ImGui::Checkbox("Enable Wireframe", &settings->enableWireframe);
	ImGui::Checkbox("Disable Gamma Correction", &settings->disableGammaCorrection);

	int* drawModePtr = (int*)&settings->deferredDrawMode;
	ImGui::Text("Deferred Draw Mode");
	ImGui::RadioButton("All", drawModePtr, 0);
	ImGui::RadioButton("Position", drawModePtr, 1);
	ImGui::RadioButton("Normal", drawModePtr, 2);
	ImGui::RadioButton("Color", drawModePtr, 3);
	ImGui::RadioButton("Specular", drawModePtr, 4);
	ImGui::RadioButton("DepthMap", drawModePtr, 5);
	ImGui::RadioButton("AmbientOcclusion", drawModePtr, 6);

	//for (auto i = 0; i < manager.materialCount; i++) {
	//	ImGui::Text("Material: %d", i);
	//	ImGui::SameLine();
	//	ImGui::PushID(i);
	//	if (ImGui::Button("Show Alpha Map")) {
	//		GLint last_program;
	//		glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
	//		BindShader(shader);
	//		glUniform1i(debugAlphaMapIndexLocation, i);
	//		glUseProgram(last_program);
	//	}
	//	ImGui::PopID();
	//}

	//if (ImGui::Button("Clear Alpha Map")) {
	//	GLint last_program;
	//	glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
	//	BindShader(manager.shaderID);
	//	glUniform1i(debugAlphaMapIndexLocation, -1);
	//	glUseProgram(last_program);
	//}
}


void ShowCameraDebugInfo (const Camera& camera) {
	ImGui::Begin("Camera Debug Info");
	ImGui::Text("Position: %f, %f, %f", camera.position.x, camera.position.y, camera.position.z);
	ImGui::Text("Yaw: %f", camera.yaw);
	ImGui::Text("Pitch %f", camera.pitch);
	ImGui::Text("Viewport: %f, %f", camera.viewportWidth, camera.viewportHeight);
	ImGui::End();
}


void ShowLightDebugInfo(const DirectionalLight& light) {
	ImGui::Begin("Light Paramaters");
	ImGui::SliderFloat3("Direction", (float*)&light.direction, -1.0f, 1.0f);
	ImGui::ColorEdit3("Color", (float*)&light.color.x);
	ImGui::SliderFloat("Ambient", (float*)&light.ambient, 0.0f, 1.0f);
	ImGui::End();
}


#if 0

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

#endif
