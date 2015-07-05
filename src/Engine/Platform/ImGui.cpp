#include "ImGui.hpp"

static void GUIInit() {

}

static void GUIBeginFrame() {
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2((float)PlatformGetWidth() (float)PlatformGetHeight());
	io.DeltaTime = PlatformGetDeltaTime();
	io.MousePos = ImVec2((float)PlatformGetCursorX(), (float)PlatformGetCursorY());
	io.MouseWheel = PlatformGetMouseWheel();

	for(uint32 i = 0; i < 3; i++) {
		io.MouseDown[i] = PlatformGetButtonDown(i);
	}
	ImGui::NewFrame();

// Hide OS mouse cursor if ImGui is drawing it
	//glfwSetInputMode(g_Window, GLFW_CURSOR, io.MouseDrawCursor ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL);
}
