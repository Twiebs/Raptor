#include <Core/Platform.h>
#include <Graphics/imgui.h>
#include <GL/glew.h>

void MainLoop(double deltaTime) {
    if (PlatformGetKey(KEY_ESCAPE)) {
        PlatformExit();
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ImGui::BeginFrame();
    ImGui::Text("Hello World");
    ImGui::EndFrame();

}

int main() {
    PlatformCreate("SpriteTest");
    ImGui::Init();
    PlatformRun(MainLoop);
}