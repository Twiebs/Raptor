
#include <GL/glew.h>
#include <Platform/Platform.hpp>
#include <Utils/IO.hpp>

#include <Core/Common.hpp>
#include <Graphics/DebugRenderer.hpp>

#include <ECS/ECSManager.hpp>
#include <Base/RenderSystem2D.hpp>

#include <Core/InputService.hpp>

#include <imgui.h>

#if __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

static GLuint       g_FontTexture = 0;
static int          g_ShaderHandle = 0, g_VertHandle = 0, g_FragHandle = 0;
static int          g_AttribLocationTex = 0, g_AttribLocationProjMtx = 0;
static int          g_AttribLocationPosition = 0, g_AttribLocationUV = 0, g_AttribLocationColor = 0;
static size_t       g_VboSize = 0;
static unsigned int g_VboHandle = 0, g_VaoHandle = 0;

std::string vertexShaderSource =
"attribute vec2 position;"
"attribute vec2 uv;"
"varying vec2 fragUV;"
"void main() {"
	"fragUV = uv;"
	"gl_Position = vec4(position.xy, 0.0, 1.0);"
"}";

std::string fragmentShaderSource =
"precision mediump float;"
"varying vec2 fragUV;"
"uniform sampler2D sampler;"
"void main() {"
	"gl_FragColor = texture2D(sampler, fragUV);"
"}";

GLuint vertexArrayID;
GLuint vertexBufferID;
GLuint elementBufferID;
GLuint textureID;
GLSLProgram* shader;

DebugRenderer* renderer;

ECSManager* ecs;
bool running = false;

const float vertices[] {
	-0.5f, -0.5f,
	0.5f, -0.5f,
	0.5f, 0.5f,
	-0.5f, 0.5f,
};

const uint32 indices[] {
	0, 3, 2,
	0, 2, 1
};

struct Vert {
	Vector2 position;
	Vector2 uv;
};


Vert verticesWithStruct[4] {
	{ Vector2(-0.5f, -0.5f)/*, Vector2(0.0f, 0.0f)*/ },
	{ Vector2(0.5f, -0.5f)/*, Vector2(1.0f, 0.0f)*/ },
	{ Vector2(0.5f, 0.5f)/*, Vector2(1.0f, 1.0f) */},
	{ Vector2(-0.5f, 0.5f)/*, Vector2(0.0f, 1.0f) */}
};


void DEBUGDrawTexture(GLuint textureID, float x, float y, float width, float height) {
	Vector2 vectorVerts[] {
		Vector2(x, y),
			Vector2(0.0, 0.0),

			Vector2(x + width, y),
			Vector2(1.0, 0.0),

			Vector2(x + width, y + height),
			Vector2(1.0, 1.0),

			Vector2(x, y + height),
			Vector2(0.0, 1.0)
	};

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vert) * 4, (GLvoid*)vectorVerts);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(vertexArrayID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glBindVertexArray(vertexArrayID);
}

void DEBUGFillRectWithStruct(float x, float y, float width, float height) {
	Vector2 vectorVerts[] {
		Vector2(x, y),
		Vector2(0.0, 0.0),

		Vector2(x + width, y),
		Vector2(1.0, 0.0),

		Vector2(x + width, y + height),
		Vector2(1.0, 1.0),

		Vector2(x, y + height),
		Vector2(0.0, 1.0)
	};

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vert) * 4, (GLvoid*)vectorVerts);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(vertexArrayID);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void DEBUGInitRendererWithStruct() {
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	glGenBuffers(1, &vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vert) * 4, vertices, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vert), (GLvoid*)offsetof(Vert, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vert), (GLvoid*)offsetof(Vert, uv));
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &elementBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32) * 6, indices, GL_DYNAMIC_DRAW);

	glBindVertexArray(0);

	shader = DEBUGLoadShaderFromSource(vertexShaderSource, fragmentShaderSource);
}



void DEBUGFillRect(float32 x, float32 y, float32 width, float32 height) {
	float newVerts[] {
			x, y,
			x + width, y,
			x + width, y + height,
			x, y + height
	};

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 2 * 4, newVerts);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(vertexArrayID);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void DEBUGInitRenderer() {
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	glGenBuffers(1, &vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * 4, vertices, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &elementBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32) * 6, indices, GL_DYNAMIC_DRAW);

	glBindVertexArray(0);

	shader = DEBUGLoadShaderFromSource(vertexShaderSource, fragmentShaderSource);
}

void MainLoop () {
#ifdef __EMSCRIPTEN__
	static double lastTime = emscripten_get_now();
	static double frameTime = emscripten_get_now();
	double currentTime = emscripten_get_now();
#else
	static double lastTime = 0;
	static double frameTime = 0;
	double currentTime = 0;
#endif

	double deltaTime =  currentTime - lastTime;
	lastTime - currentTime;

	running = PlatformHandleInput();

	static float x = -0.5f;
	static float dt = 0.0f;
	float xpos =  x * sin(dt);
	dt += 0.1f;

	PlatformBeginFrame();
	shader->Use();
	//DEBUGDrawTexture(textureID, -0.5f, -0.5f, 1.0f, 1.0f);
	DEBUGFillRectWithStruct(xpos, -0.5f, 1.0f, 1.0f);
	//ecs->Update(deltaTime);
	//Render Phase
	//DEBUGFillRect(0.75f, 0.0f, 0.1f, 1.75f);
	//renderer->Begin();
	//renderer->PushRect(-0.5f, -0.5f, 1.0f, 1.0f);
	//renderer->End();
	PlatformEndFrame();
}

void ImGui_ImplGlfwGL3_CreateFontsTexture()
{
    ImGuiIO& io = ImGui::GetIO();

    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bits for OpenGL3 demo because it is more likely to be compatible with user's existing shader.

    glGenTextures(1, &g_FontTexture);
    glBindTexture(GL_TEXTURE_2D, g_FontTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    // Store our identifier
    io.Fonts->TexID = (void *)(intptr_t)g_FontTexture;

    // Cleanup (don't clear the input data if you want to append new fonts later)
    io.Fonts->ClearInputData();
    io.Fonts->ClearTexData();
}

bool CreateShaderObjects()
{
    const GLchar *vertex_shader =
        "#version 330\n"
        "uniform mat4 ProjMtx;\n"
        "in vec2 Position;\n"
        "in vec2 UV;\n"
        "in vec4 Color;\n"
        "out vec2 Frag_UV;\n"
        "out vec4 Frag_Color;\n"
        "void main()\n"
        "{\n"
        "	Frag_UV = UV;\n"
        "	Frag_Color = Color;\n"
        "	gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
        "}\n";

    const GLchar* fragment_shader =
        "#version 330\n"
        "uniform sampler2D Texture;\n"
        "in vec2 Frag_UV;\n"
        "in vec4 Frag_Color;\n"
        "out vec4 Out_Color;\n"
        "void main()\n"
        "{\n"
        "	Out_Color = Frag_Color * texture( Texture, Frag_UV.st);\n"
        "}\n";

    g_ShaderHandle = glCreateProgram();
    g_VertHandle = glCreateShader(GL_VERTEX_SHADER);
    g_FragHandle = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(g_VertHandle, 1, &vertex_shader, 0);
    glShaderSource(g_FragHandle, 1, &fragment_shader, 0);
    glCompileShader(g_VertHandle);
    glCompileShader(g_FragHandle);
    glAttachShader(g_ShaderHandle, g_VertHandle);
    glAttachShader(g_ShaderHandle, g_FragHandle);
    glLinkProgram(g_ShaderHandle);

    g_AttribLocationTex = glGetUniformLocation(g_ShaderHandle, "Texture");
    g_AttribLocationProjMtx = glGetUniformLocation(g_ShaderHandle, "ProjMtx");
    g_AttribLocationPosition = glGetAttribLocation(g_ShaderHandle, "Position");
    g_AttribLocationUV = glGetAttribLocation(g_ShaderHandle, "UV");
    g_AttribLocationColor = glGetAttribLocation(g_ShaderHandle, "Color");

    glGenBuffers(1, &g_VboHandle);

    glGenVertexArrays(1, &g_VaoHandle);
    glBindVertexArray(g_VaoHandle);
    glBindBuffer(GL_ARRAY_BUFFER, g_VboHandle);
    glEnableVertexAttribArray(g_AttribLocationPosition);
    glEnableVertexAttribArray(g_AttribLocationUV);
    glEnableVertexAttribArray(g_AttribLocationColor);

#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
    glVertexAttribPointer(g_AttribLocationPosition, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, pos));
    glVertexAttribPointer(g_AttribLocationUV, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, uv));
    glVertexAttribPointer(g_AttribLocationColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, col));
#undef OFFSETOF
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    ImGui_ImplGlfwGL3_CreateFontsTexture();

    return true;
}

static void ImGUIRenderDrawLists(ImDrawList** const cmd_lists, int cmd_lists_count) {
	if (cmd_lists_count == 0)
		return;

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

  // Setup orthographic projection matrix
  const float width = ImGui::GetIO().DisplaySize.x;
  const float height = ImGui::GetIO().DisplaySize.y;
  const float ortho_projection[4][4] =
  {
      { 2.0f/width,	0.0f,			0.0f,		0.0f },
      { 0.0f,			2.0f/-height,	0.0f,		0.0f },
      { 0.0f,			0.0f,			-1.0f,		0.0f },
      { -1.0f,		1.0f,			0.0f,		1.0f },
  };

	glUseProgram(g_ShaderHandle);
    glUniform1i(g_AttribLocationTex, 0);
    glUniformMatrix4fv(g_AttribLocationProjMtx, 1, GL_FALSE, &ortho_projection[0][0]);

    // Grow our buffer according to what we need
    size_t total_vtx_count = 0;
    for (int n = 0; n < cmd_lists_count; n++)
        total_vtx_count += cmd_lists[n]->vtx_buffer.size();
    glBindBuffer(GL_ARRAY_BUFFER, g_VboHandle);
    size_t needed_vtx_size = total_vtx_count * sizeof(ImDrawVert);
    if (g_VboSize < needed_vtx_size)
    {
        g_VboSize = needed_vtx_size + 5000 * sizeof(ImDrawVert);  // Grow buffer
        glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)g_VboSize, NULL, GL_STREAM_DRAW);
    }

    // Copy and convert all vertices into a single contiguous buffer
    unsigned char* buffer_data = (unsigned char*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    if (!buffer_data)
        return;
    for (int n = 0; n < cmd_lists_count; n++)
    {
        const ImDrawList* cmd_list = cmd_lists[n];
        memcpy(buffer_data, &cmd_list->vtx_buffer[0], cmd_list->vtx_buffer.size() * sizeof(ImDrawVert));
        buffer_data += cmd_list->vtx_buffer.size() * sizeof(ImDrawVert);
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(g_VaoHandle);

    int cmd_offset = 0;
    for (int n = 0; n < cmd_lists_count; n++)
    {
        const ImDrawList* cmd_list = cmd_lists[n];
        int vtx_offset = cmd_offset;
        const ImDrawCmd* pcmd_end = cmd_list->commands.end();
        for (const ImDrawCmd* pcmd = cmd_list->commands.begin(); pcmd != pcmd_end; pcmd++)
        {
            if (pcmd->user_callback)
            {
                pcmd->user_callback(cmd_list, pcmd);
            }
            else
            {
                glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->texture_id);
                glScissor((int)pcmd->clip_rect.x, (int)(height - pcmd->clip_rect.w), (int)(pcmd->clip_rect.z - pcmd->clip_rect.x), (int)(pcmd->clip_rect.w - pcmd->clip_rect.y));
                glDrawArrays(GL_TRIANGLES, vtx_offset, pcmd->vtx_count);
            }
            vtx_offset += pcmd->vtx_count;
        }
        cmd_offset = vtx_offset;
    }

    // Restore modified state
    glBindVertexArray(0);
    glUseProgram(last_program);
    glDisable(GL_SCISSOR_TEST);
    glBindTexture(GL_TEXTURE_2D, last_texture);
}

int main () {
	PlatformInit("Raptor GLTest", 1280, 720, false);
	running = true;

	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize.x = 1280.0f;
	io.DisplaySize.y = 720.0f;
	io.DeltaTime = 1.0f/60.0f;
	io.IniFilename = "imgui.ini";
	io.KeyMap[ImGuiKey_Tab] = KEY_TAB;
	io.KeyMap[ImGuiKey_LeftArrow] = KEY_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = KEY_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = KEY_UP;
	io.KeyMap[ImGuiKey_DownArrow] = KEY_DOWN;
	// io.KeyMap[ImGuiKey_PageUp] = KEY_PAGE_UP;
	// io.KeyMap[ImGuiKey_PageDown] = KEY_PAGE_DOWN;
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
	io.RenderDrawListsFn  = ImGUIRenderDrawLists;

	CreateShaderObjects();

	// io.SetClipboardTextFn = ImGui_ImplGlfwGL3_SetClipboardText;
	// io.GetClipboardTextFn = ImGui_ImplGlfwGL3_GetClipboardText;

	renderer = new DebugRenderer();

	//TODO specifiy component registration params inside the systems?
	// ecs = new ECSManager();
	// ecs->RegisterComponent<Transform2D>(0);
	// ecs->RegisterComponent<SpriteComponent>(0);
	// ecs->RegisterComponent<TextComponent>(0);
	// ecs->Initalize();

	textureID = DEBUGLoadTexture("Assets/null.png");
	// auto renderSystem = ecs->CreateSystem<RenderSystem2D>();
	// //renderSystem->SetProjectionMatrix(Matrix4::Ortho(0, 1270, 0, 720, 0.1, 1000, 1.0));
	// auto entity = ecs->CreateEntity();
	// auto sprite =ecs->CreateComponent<SpriteComponent>(entity);
	// sprite->x = 0;
	// sprite->y = 0;
	// sprite->width = 512;
	// sprite->height = 512;
	// sprite->textureID = textureID;

	DEBUGInitRendererWithStruct();
	//DEBUGInitRendererWithVert();

#ifndef __EMSCRIPTEN__
	LOG_INFO("The main loop is running...");
	while(running) {
		MainLoop();
	}
#else
	emscripten_set_main_loop(MainLoop, 0, 1);
#endif

}
