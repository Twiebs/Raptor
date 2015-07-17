#include <vector>
#include <Core/Application.hpp>

#include <Graphics/GUI.hpp>
#include <Graphics/Texture.hpp>

#define STB_RECT_PACK_IMPLEMENTATION
#include <imgui/stb_rect_pack.h>

GUIContext gGUIContext;

#define FOLIAGE_COUNT 47
void MainLoop(Application* app) {
	GUIBeginFrame(&gGUIContext, app);
	static bool atlasPacked = false;
	static bool atlasLoaded = false;
	ImGui::SetNextWindowPos(ImVec2(app->GetWidth() - 270, (app->GetHeight() * 0.5f) - ((app->GetHeight() - 20)* 0.5f)));
	ImGui::Begin("TexturePacker", nullptr, ImVec2(250, app->GetHeight() - 20), 0.3f, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_NoResize);
	if (ImGui::Button("Pack TextureAtlas")) {
		auto textureCount = FOLIAGE_COUNT;
		std::string directory = std::string(ASSET_DIR) + std::string("textures/foliage/");
		std::vector<std::string> filenames;
		for (auto i = 0; i < textureCount; i++) {
			filenames.push_back(directory + "tree_" + std::to_string(i + 1) + ".png");
		}

		stbrp_context rpContext;
		stbrp_node* nodes = new stbrp_node[textureCount];
		stbrp_rect* rects = new stbrp_rect[textureCount];
		Pixmap* pixmaps = new Pixmap[textureCount];
		for (auto i = 0; i < textureCount; i++) {
			LoadPixmapFromFile(&pixmaps[i], filenames[i]);
			rects[i] = { 0, pixmaps[i].width, pixmaps[i].height };
		}

		TextureAtlas atlas;
		atlas.width = 512;
		atlas.height = 512;
		atlas.pixels = new U8[atlas.width * atlas.height * 4];
		memset(atlas.pixels, 0, atlas.width * atlas.height * 4);
		atlas.regionCount = textureCount;
		atlas.regions = new AtlasRegion[textureCount];

		stbrp_init_target(&rpContext, atlas.width, atlas.height, nodes, textureCount);
		stbrp_pack_rects(&rpContext, rects, textureCount);
		for (auto i = 0; i < textureCount; i++) {
			auto& rect = rects[i];
			if (!rect.was_packed) {
				LOG_ERROR("COULD NOT PACK RECT");
				continue;
			}
			atlas.regions[i] = { rect.x, rect.y, rect.x + pixmaps[i].width, rect.y + pixmaps[i].height };
			WritePixelsHack(atlas.pixels, rect.x, rect.y, atlas.width, atlas.height, pixmaps[i].pixels, pixmaps[i].width, pixmaps[i].height);
		}

		WriteTextureAtlasToFile(&atlas, directory + "trees.atlas");
		delete[] atlas.pixels;
		delete[] atlas.regions;
		delete[] rects;
		delete[] nodes;
		delete[] pixmaps;
		atlasPacked = true;
	}
	ImGui::End();

	if (atlasPacked) {
		static bool atlasLoaded = false;
		static TextureAtlas atlas;
		static GLuint atlasTextureID;
		if (!atlasLoaded) {
			std::string directory = std::string(ASSET_DIR) + std::string("textures/foliage/");
			ReadTextureAtlasFromFile(&atlas, directory + "trees.atlas");
			//memset(atlas.pixels, 255, atlas.width * atlas.height * 4);
			atlasTextureID = CreateTextureFromPixels(atlas.width, atlas.height, atlas.pixels);
			//atlasTextureID = CreateTextureFromFile(ASSET("textures/foliage/trees.png.backup"));
			atlasLoaded = true;
		}

		auto atlasWindowWidth = atlas.width + 20;
		auto atlasWindowHeight = atlas.height + 20;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));
		ImGui::SetNextWindowPos(ImVec2((app->GetWidth() * 0.5f) - (atlasWindowWidth * 0.5f), (app->GetHeight() * 0.5f) - (atlasWindowHeight * 0.5f)));
		ImGui::SetNextWindowSize(ImVec2(atlasWindowWidth, atlasWindowHeight));
		ImGui::Begin("TextureAtlas", nullptr, ImVec2((app->GetWidth() * 0.5f) - (atlasWindowWidth * 0.5f), (app->GetHeight() * 0.5f) - (atlasWindowHeight * 0.5f)), 0.3f,  ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoTitleBar );
		ImGui::Image((ImTextureID)atlasTextureID, ImVec2(atlas.width, atlas.height), ImVec2(0, 0), ImVec2(1,1), ImVec4(1, 1, 1, 1), ImVec4(0.55, 0.55, 0.55, 0.55));
		ImGui::End();
		ImGui::PopStyleVar();
	} 
	GUIEndFrame();
}

#undef main

#if 1
int main() {
	Application app("Raptor Tools");
	GUIContextInit(&gGUIContext, app.GetWidth(), app.GetHeight());
	app.Run(MainLoop);
}
#endif
