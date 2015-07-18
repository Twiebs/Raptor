#include "GUI.hpp"


void ImGUIRenderDrawLists(ImDrawList** const drawList, int drawListCount) {
	if (drawListCount == 0)
		return;

	GUIContext* imGuiContext = (GUIContext*)ImGui::GetIO().UserData;

	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_SCISSOR_TEST);
	glActiveTexture(GL_TEXTURE0);

	uint32 vertexCount = 0;
	for (int i = 0; i < drawListCount; i++) {
		vertexCount += drawList[i]->vtx_buffer.size();
	}

	glBindBuffer(GL_ARRAY_BUFFER, imGuiContext->vertexBufferID);
	uint32 requiredMemorySize = vertexCount * sizeof(ImDrawVert);
	if (imGuiContext->vertexMemorySize < requiredMemorySize) {
		imGuiContext->vertexMemorySize = requiredMemorySize + 5000 * sizeof(ImDrawVert);  // Grow buffer
		glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)imGuiContext->vertexMemorySize, NULL, GL_DYNAMIC_DRAW);
	}

	uint32 offset = 0;
	for (uint32 i = 0; i < drawListCount; i++) {
		const ImDrawList* drawCall = drawList[i];
		uint32 size = drawCall->vtx_buffer.size() * sizeof(ImDrawVert);
		glBufferSubData(GL_ARRAY_BUFFER, offset, size, &drawCall->vtx_buffer[0]);
		offset += size;
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(imGuiContext->vertexArrayID);

	uint32 vertexOffset = 0;
	for (uint32 i = 0; i < drawListCount; i++) {
		const ImDrawList* drawCall = drawList[i];
		const ImDrawCmd* cmdEnd = drawCall->commands.end();
		for (const ImDrawCmd* cmd = drawCall->commands.begin(); cmd != cmdEnd; cmd++) {
			if (cmd->user_callback) {
				cmd->user_callback(drawCall, cmd);
			}
			else {
				glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)cmd->texture_id);
				// glScissor((int)cmd->clip_rect.x, (int)(app.GetHeight() - cmd->clip_rect.w), (int)(cmd->clip_rect.z - cmd->clip_rect.x), (int)(cmd->clip_rect.w - cmd->clip_rect.y));
				glDrawArrays(GL_TRIANGLES, vertexOffset, cmd->vtx_count);
			}
			vertexOffset += cmd->vtx_count;
		}
	}
	glBindVertexArray(0);
	glDisable(GL_SCISSOR_TEST);
}


void GUISetDisplaySize(GUIContext* context, U32 width, U32 height) {
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2((float)width, (float)height);
	glUseProgram(context->shaderProgramID);
	Matrix4 projection = Matrix4::Ortho(0.0f, (F32)width, 0.0f, (F32)height, -1.0f);
	glUniformMatrix4fv(context->projectionUniformLoc, 1, GL_FALSE, &projection[0][0]);
}

void GUIBeginFrame(GUIContext* context, Application* app) {
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = app->GetDeltaTime() > 0.0 ? app->GetDeltaTime() : 1.0f / 60.0f;
	io.MousePos = ImVec2((F32)app->GetCursorX(), (F32)app->GetCursorY());
	io.MouseWheel = app->GetMouseWheel();
	io.MouseDown[0] = app->IsButtonDown(MOUSE_BUTTON_LEFT);
	io.KeyAlt = app->IsKeyDown(KEY_LALT | KEY_RALT);
	io.KeyCtrl = app->IsKeyDown(KEY_LCTRL | KEY_RCTRL);
	io.KeyShift = app->IsKeyDown(KEY_LSHIFT | KEY_RSHIFT);
	glUseProgram(context->shaderProgramID);
	ImGui::NewFrame();
}

void GUIEndFrame() {
	ImGui::Render();
}

bool GUIContextInit(GUIContext* imGuiContext, U32 viewportWidth, U32 viewportHeight) {
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

	ImGuiIO& io = ImGui::GetIO();
	io.UserData = imGuiContext;
	io.DisplaySize.x = 1280.0f;
	io.DisplaySize.y = 720.0f;
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
	io.RenderDrawListsFn = ImGUIRenderDrawLists;
	// io.SetClipboardTextFn = ImGui_ImplGlfwGL3_SetClipboardText;
	// io.GetClipboardTextFn = ImGui_ImplGlfwGL3_GetClipboardText;


	imGuiContext->shaderProgramID = LoadShaderFromSource(vertex_shader, fragment_shader);
	imGuiContext->samplerUniformLoc = GetUniformLocation(imGuiContext->shaderProgramID, "sampler");
	imGuiContext->projectionUniformLoc = GetUniformLocation(imGuiContext->shaderProgramID, "projection");

	glGenVertexArrays(1, &imGuiContext->vertexArrayID);
	glBindVertexArray(imGuiContext->vertexArrayID);
	glGenBuffers(1, &imGuiContext->vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, imGuiContext->vertexBufferID);

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

	glGenTextures(1, &imGuiContext->fontTexture);
	glBindTexture(GL_TEXTURE_2D, imGuiContext->fontTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	Matrix4 projection = Matrix4::Ortho(0.0f, (F32)viewportWidth, 0.0f, (F32)viewportHeight, -1.0f);
	glUseProgram(imGuiContext->shaderProgramID);
	glUniform1i(imGuiContext->samplerUniformLoc, 0);
	glUniformMatrix4fv(imGuiContext->projectionUniformLoc, 1, GL_FALSE, &projection[0][0]);

	// Store our identifier
	io.Fonts->TexID = (void *)(intptr_t)imGuiContext->fontTexture;

	// Cleanup (don't clear the input data if you want to append new fonts later)
	io.Fonts->ClearInputData();
	io.Fonts->ClearTexData();
	return true;
}

