#include "imgui.h"
#include "imgui_user.h"
#include <GL/glew.h>

#include <Core/Common.hpp>
#include <Core/Platform.h>
#include <Math/Matrix4.hpp>
#include <Graphics/GLSLProgram.hpp>

global_variable GLuint global_vertexArrayID, global_vertexBufferID, global_elementBufferID;
global_variable GLuint global_shaderID, global_samplerUniformLoc, global_projectionUniformLoc;
global_variable GLuint global_fontTextureID;

static void ImGuiRenderDrawLists(ImDrawData* draw_data)
{
	// Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled
	GLint last_program, last_texture;
	glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_SCISSOR_TEST);
	glActiveTexture(GL_TEXTURE0);

	// Handle cases of screen coordinates != from framebuffer coordinates (e.g. retina displays)
	ImGuiIO& io = ImGui::GetIO();
	float fb_height = io.DisplaySize.y * io.DisplayFramebufferScale.y;
	draw_data->ScaleClipRects(io.DisplayFramebufferScale);

	// Setup orthographic projection matrix
	const float ortho_projection[4][4] =
	{
		{ 2.0f / io.DisplaySize.x, 0.0f,                   0.0f, 0.0f },
		{ 0.0f,                  2.0f / -io.DisplaySize.y, 0.0f, 0.0f },
		{ 0.0f,                  0.0f,                  -1.0f, 0.0f },
		{ -1.0f,                  1.0f,                   0.0f, 1.0f },
	};
	glUseProgram(global_shaderID);
	glUniform1i(global_samplerUniformLoc, 0);
	glUniformMatrix4fv(global_projectionUniformLoc, 1, GL_FALSE, &ortho_projection[0][0]);
	glBindVertexArray(global_vertexArrayID);

	for (int n = 0; n < draw_data->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = draw_data->CmdLists[n];
		const ImDrawIdx* idx_buffer_offset = 0;

		glBindBuffer(GL_ARRAY_BUFFER, global_vertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.size() * sizeof(ImDrawVert), (GLvoid*)&cmd_list->VtxBuffer.front(), GL_STREAM_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, global_elementBufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.size() * sizeof(ImDrawIdx), (GLvoid*)&cmd_list->IdxBuffer.front(), GL_STREAM_DRAW);

		for (const ImDrawCmd* pcmd = cmd_list->CmdBuffer.begin(); pcmd != cmd_list->CmdBuffer.end(); pcmd++)
		{
			if (pcmd->UserCallback)
			{
				pcmd->UserCallback(cmd_list, pcmd);
			} else
			{
				glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
				glScissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
				glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, GL_UNSIGNED_SHORT, idx_buffer_offset);
			}
			idx_buffer_offset += pcmd->ElemCount;
		}
	}

	// Restore modified state
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glUseProgram(last_program);
	glDisable(GL_SCISSOR_TEST);
	glBindTexture(GL_TEXTURE_2D, last_texture);
}

int ImGui::Init() {
#ifndef __EMSCRIPTEN__
	const GLchar *vertex_shader =
		"#version 330\n"
		"layout (location = 0) in vec2 position;"
		"layout (location = 1) in vec2 uv;"
		"layout (location = 2) in vec4 color;"
		"out vec2 fragUV;"
		"out vec4 fragColor;"
		"uniform mat4 projection;"
		"void main() {"
		"	fragUV = uv;"
		"	fragColor = color;"
		"	gl_Position = projection * vec4(position.xy, 0.0, 1.0);"
		"};";

	const GLchar* fragment_shader =
		"#version 330\n"
		"in vec2 fragUV;"
		"in vec4 fragColor;"
		"out vec4 outColor;"
		"uniform sampler2D sampler;"
		"void main() {"
		"	outColor = fragColor * texture(sampler, fragUV);"
		"}";

#else
	const GLchar* vertex_shader =
		"attribute vec2 position;"
		"attribute vec2 uv;"
		"attribute vec4 color;"
		"varying vec2 fragUV;"
		"varying vec4 fragColor;"
		"uniform mat4 projection;"
		"void main() {"
		"	fragUV = uv;"
		"	fragColor = color;"
		"	gl_Position = projection * vec4(position.xy, 0.0, 1.0);"
		"}";

	const GLchar* fragment_shader =
		"precision mediump float;"
		"varying vec2 fragUV;"
		"varying vec4 fragColor;"
		"uniform sampler2D sampler;"
		"void main() {"
		"	gl_FragColor = fragColor * texture2D(sampler, fragUV);"
		"}";

#endif
	int viewportWidth, viewportHeight;
	PlatformGetSize(&viewportWidth, &viewportHeight);
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize.x = viewportWidth;
	io.DisplaySize.y = viewportHeight;
	io.DeltaTime = 1.0f / 60.0f;
	io.IniFilename = "imgui.ini";
	io.KeyMap[ImGuiKey_Tab] = KEY_TAB;
	io.KeyMap[ImGuiKey_LeftArrow] = KEY_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = KEY_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = KEY_UP;
	io.KeyMap[ImGuiKey_DownArrow] = KEY_DOWN;
	//io.KeyMap[ImGuiKey_PageUp] = KEY_PAGE_UP;
	//io.KeyMap[ImGuiKey_PageDown] = KEY_PAGE_DOWN;
	io.KeyMap[ImGuiKey_Home] = KEY_HOME;
	io.KeyMap[ImGuiKey_End] = KEY_END;
	io.KeyMap[ImGuiKey_Delete] = KEY_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = KEY_BACKSPACE;
	io.KeyMap[ImGuiKey_Enter] = KEY_ENTER;
	io.KeyMap[ImGuiKey_Escape] = KEY_ESCAPE;
	io.KeyMap[ImGuiKey_A] = KEY_A;
	io.KeyMap[ImGuiKey_C] = KEY_C;

	io.KeyMap[ImGuiKey_V] = KEY_V;
	io.KeyMap[ImGuiKey_X] = KEY_X;
	io.KeyMap[ImGuiKey_Y] = KEY_Y;
	io.KeyMap[ImGuiKey_Z] = KEY_Z;
	io.RenderDrawListsFn = ImGuiRenderDrawLists;
	// io.SetClipboardTextFn = ImGui_ImplGlfwGL3_SetClipboardText;
	// io.GetClipboardTextFn = ImGui_ImplGlfwGL3_GetClipboardText;

	global_shaderID = CreateShaderFromSource(vertex_shader, fragment_shader);
	global_samplerUniformLoc = GetUniformLocation(global_shaderID, "sampler");
	global_projectionUniformLoc = GetUniformLocation(global_shaderID, "projection");

	glGenVertexArrays(1, &global_vertexArrayID);
	glGenBuffers(1, &global_vertexBufferID);
	glGenBuffers(1, &global_elementBufferID);

	glBindVertexArray(global_vertexArrayID);
	glBindBuffer(GL_ARRAY_BUFFER, global_vertexBufferID);
	
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)offsetof(ImDrawVert, pos));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)offsetof(ImDrawVert, uv));
	glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)offsetof(ImDrawVert, col));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	uint8* pixels;
	int width, height;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bits for OpenGL3 demo because it is more likely to be compatible with user's existing shader.

	glGenTextures(1, &global_fontTextureID);
	glBindTexture(GL_TEXTURE_2D, global_fontTextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	Matrix4 projection = Matrix4::Ortho(0.0f, (F32)width, 0.0f, (F32)height, -1.0f);
	glUseProgram(global_shaderID);
	glUniform1i(global_samplerUniformLoc, 0);
	glUniformMatrix4fv(global_projectionUniformLoc, 1, GL_FALSE, &projection[0][0]);

	// Store our identifier
	io.Fonts->TexID = (void *)(intptr_t)global_fontTextureID;

	// Cleanup (don't clear the input data if you want to append new fonts later)
	io.Fonts->ClearInputData();
	io.Fonts->ClearTexData();
	return 0;
}

void ImGui::BeginFrame() {
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = PlatformGetDeltaTime();
	int x, y; PlatformGetCursorPos(&x, &y);
	io.MousePos = ImVec2((F32)x, (F32)y);
	//io.MouseWheel = app->GetMouseWheel();
	io.MouseDown[0] = PlatformGetButton(MOUSE_BUTTON_LEFT);
	io.KeyAlt = PlatformGetKey(KEY_RALT) || PlatformGetKey(KEY_LALT);
	io.KeyCtrl = PlatformGetKey(KEY_RCTRL) || PlatformGetKey(KEY_LCTRL);
	io.KeyShift = PlatformGetKey(KEY_LSHIFT) || PlatformGetKey(KEY_RSHIFT);
	glUseProgram(global_shaderID);
	ImGui::NewFrame();
}

void ImGui::EndFrame() {
	ImGui::Render();
}