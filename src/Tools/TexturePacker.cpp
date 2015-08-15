#include <vector>
#include <Core/Application.hpp>

#include <Graphics/GUI.hpp>
#include <Graphics/Texture.hpp>
#include <Platform/tinydir.h>

#define STB_RECT_PACK_IMPLEMENTATION
#include <imgui/stb_rect_pack.h>

GUIContext gGUIContext;

void KeyCallback(int keycode, bool isDown) {
	ImGuiIO& io = ImGui::GetIO();
	io.KeysDown[keycode] = isDown;
}

#define FOLIAGE_COUNT 47
void MainLoop(Application* app) {
	GUIBeginFrame(&gGUIContext, app);
	static bool atlasPacked = false;
	static bool atlasLoaded = false;
	ImGui::SetNextWindowPos(ImVec2(app->GetWidth() - 270, (app->GetHeight() * 0.5f) - ((app->GetHeight() - 20)* 0.5f)));
	ImGui::Begin("TexturePacker", nullptr, ImVec2(250, app->GetHeight() - 20), 0.3f, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_NoResize);
	
	static char inputBuffer[64];
	static char outFile[64];
	static bool packPNG = false;
	static bool packPSD = false;
	static int padding = 0;
	ImGui::InputText("OutFile", outFile, 64);
	ImGui::Separator();
	ImGui::InputText("Input Directory,", inputBuffer, 64);
	ImGui::Separator();
	ImGui::Checkbox("PNG", &packPNG);
	ImGui::Checkbox("PSD", &packPSD);
	ImGui::Separator();
	ImGui::SliderInt("Padding", &padding, 0, 12);
	ImGui::Separator();
	if (ImGui::Button("Woof TextureAtlas")) {
		auto directory = ASSET_DIR + std::string(inputBuffer);
		std::vector<std::string> filenames;
		tinydir_dir dir;
		tinydir_open(&dir, directory.c_str());
		while (dir.has_next) {
			tinydir_file file;
			tinydir_readfile(&dir, &file);
			if (!strcmp(file.extension, "png")) filenames.push_back(file.name);
			tinydir_next(&dir);
		}
		tinydir_close(&dir);

		auto textureCount = filenames.size();
		stbrp_context rpContext;
		stbrp_node* nodes = new stbrp_node[textureCount];
		stbrp_rect* rects = new stbrp_rect[textureCount];
		Pixmap* pixmaps = new Pixmap[textureCount];
		for (auto i = 0; i < textureCount; i++) {
			LoadPixmapFromFile(&pixmaps[i], std::string(inputBuffer) + "/" + filenames[i]);
			rects[i] = { 0, pixmaps[i].width + (padding * 2), pixmaps[i].height + (padding * 2) };
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
			F32 x = rect.x + padding;
			F32 y = rect.y + padding;
			atlas.regions[i] = { x / (F32)atlas.width, y / (F32)atlas.height, (F32)(x + pixmaps[i].width) / (F32)atlas.width, (F32)(y + pixmaps[i].height) / (F32)atlas.height };
			WritePixelsHack(atlas.pixels, x, y, atlas.width, atlas.height, pixmaps[i].pixels, pixmaps[i].width, pixmaps[i].height);
		}

		WriteTextureAtlasToFile(&atlas, directory + "/" + std::string(outFile) + ".atlas");
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
			LoadTextureAtlasFromFile(&atlas, directory + "trees.atlas");
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
	Application app("Raptor TexturePacker");
	app.SetKeyCallback(KeyCallback);
	GUIContextInit(&gGUIContext, app.GetWidth(), app.GetHeight());
	app.Run(MainLoop);
}
#endif
