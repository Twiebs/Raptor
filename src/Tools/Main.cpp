#include <vector>
#include <Core/Application.hpp>

#include <Graphics/GUI.hpp>
#include <Utils/IO.hpp>

#define STB_RECT_PACK_IMPLEMENTATION
#include <imgui/stb_rect_pack.h>

GUIContext gGUIContext;

#define FOLIAGE_COUNT 47
GLuint foliageTextures[FOLIAGE_COUNT];

void MainLoop(Application* app) {
	GUIBeginFrame(&gGUIContext, app);
	ImGui::BeginMainMenuBar();
	ImGui::MenuItem("File");
	ImGui::EndMainMenuBar();

	ImGui::Begin("TextureAtlas Packer", nullptr, ImVec2(0.0f, 0.0f), 0.3f, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoSavedSettings);
	ImGui::Text("Texture Size");
	ImGui::End();

	U32 textureCount = 10;
	std::string directory = std::string(ASSET_DIR) + std::string("textures/foliage/");
	std::vector<std::string> filenames;
	for(auto i = 0; i < textureCount; i++) {
		filenames.push_back(directory + "tree_" + std::to_string(i + 1) + ".png");
	}

	stbrp_context rpContext;
	stbrp_node* nodes = new stbrp_node[textureCount];
	stbrp_rect* rects = new stbrp_rect[textureCount];
	Pixmap** pixmaps = new Pixmap*[textureCount];
	stbrp_init_target(&rpContext, 1024, 1024, nodes, textureCount);
	for(auto i = 0; i < textureCount; i++) {
		pixmaps[i] = LoadPixmap(filenames[i]);
		rects[i] = { 0, pixmaps[i]->width, pixmaps[i]->height };
	}

	stbrp_pack_rects(&rpContext, rects, textureCount);
	Pixmap pixmap(1024, 1024);
	for(auto i = 0; i < textureCount; i++) {
		auto& rect = rects[i];
		memcpy(pixmap.GetPixelAddress(rect.x, rect.y), pixmaps[i]->data, pixmaps[i]->width * pixmaps[i]->height * 4);
	}

	WritePixmap(&pixmap, directory + "trees.png");


	ImGui::ShowTestWindow();
	GUIEndFrame();
}

int main() {
	Application app("Raptor Tools");
	GUIContextInit(&gGUIContext, app.GetWidth(), app.GetHeight());
	app.Run(MainLoop);
}
