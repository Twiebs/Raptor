#pragma once

#include <Core/Application.hpp>
#include <imgui/imgui.h>
#include <GL/glew.h>

#include <Math/Matrix4.hpp>
#include <Utils/IO.hpp>
#include <Graphics/GLSLProgram.hpp>

struct GUIContext {
	GLuint vertexArrayID;
	GLuint vertexBufferID;
	GLuint vertexMemorySize;
	GLuint fontTexture;
	GLuint shaderProgramID;
	GLuint projectionUniformLoc;
	GLuint samplerUniformLoc;
};


bool GUIContextInit(GUIContext* imGuiContext, U32 viewportWidth, U32 viewportHeight);
void GUISetDisplaySize(GUIContext* context, U32 width, U32 height);
void ImGUIRenderDrawLists(ImDrawList** const drawList, int drawListCount);

void GUIBeginFrame(GUIContext* context, Application* app);
void GUIEndFrame();



