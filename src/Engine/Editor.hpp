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

#include <map>

#include <Engine/GFX3D.hpp>
#include <Utils/Profiler.hpp>
#include <Graphics/Render3D.hpp>
#include <Core/Platform.h>

void ShowProfilerPanel (Profiler* profiler);
void ShowCameraDebugInfo(const Camera& camera);
void ShowLightDebugInfo(const DirectionalLight& light);
void ShowDebugRenderSettings(DebugRenderSettings* settings);
void ShowAssetBrowser();

#include <fstream>
#include <cereal/archives/json.hpp>


//void ShowFileExplorer(const std::string& directory) {
//	std::vector<
//
//
//}

void ImportAsset(const std::string& filename, AssetManifest* manifest);


// TODO
// This is not a final implementation
// This is a slow version
struct HotReloadEntry {
	std::string filename;
	FileWriteTime time;
	U32 flags;
	U32 id;
};

class HotReloadWatchList {
public:
	HotReloadWatchList();
	~HotReloadWatchList();

	void AddWatch(const std::string& filename, U32 flags, U32 entryID);
	void RemoveWatch(const std::string& filename);
	U32 PollHotreloadState(HotReloadEntry** outEntry);

private:
	// std::vector<HotReloadEntry> entries;
	// std::vector<U32> watchesToRemove;
	// std::unordered_map<std::string, S32> filenameIndexMap;
	std::unordered_map<std::string, HotReloadEntry> filenameEntryMap;
	std::unordered_map<std::string, HotReloadEntry>::iterator internalPollIterator;
	
	// U32 internalPollIndex;
};

struct EditorOfflineState {
	// std::unordered_map<std::string, FileWriteTime> writeTimesForAssets;
	EditorOfflineState();
	~EditorOfflineState();
};

struct AssetBrowser {
	S32 selectedAssetType = 0;
	S32 selectedAssetIndex = -1;
	AssetManager* manager;
	AssetManifest* manifest;
	HotReloadWatchList hotreloadWatchList;
};

struct EditorWindow {
	EditorOfflineState state;
	AssetBrowser assetBrowser;
	S32 selectedWindowIndex;
	DebugRenderSettings renderSettings;
};

void CheckAssetFolderForChanges(EditorWindow* editor);

void ShowEditorWindow (EditorWindow* editor);
void ProcessEditorKeypress (EditorWindow* editor, int keycode);

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

