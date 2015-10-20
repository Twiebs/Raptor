#include <Core/Platform.h>
#include <Graphics/imgui.h>

int main () {
    PlatformCreate("Asset Explorer");
    ImGui::Init();

    PlatformRun([](float deltaTime) {
        ImGui::BeginFrame();
        ImGui::Begin("Asset Viewer");
        ImGui::End();
        ImGui::EndFrame();
    });
}