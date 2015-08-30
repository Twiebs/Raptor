#include <Core/Platform.h>
#include <Graphics/imgui.h>
#include <GL/glew.h>

#include <Math/Vector3.hpp>
#include <Engine/Assets.hpp>

using namespace Raptor;

global_variable Camera global_camera;

void MainLoop(F64 deltaTime) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (PlatformGetKey(KEY_ESCAPE)) PlatformExit();

	UpdateCamera(global_camera);
	ImGui::BeginFrame();
	ImGui::Begin("Test Window");
	ImGui::Text("Woof! Woof! Woof!");
	ImGui::Button("Click me bitch!");
	ImGui::End();
	ImGui::EndFrame();
}

int main() {
	PlatformCreate("Manhattan Legacy");
	ImGui::Init();
	LoadModel("models/warhead.obj");

	PlatformRun(MainLoop);

	return 0;
}