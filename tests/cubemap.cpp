

#include <Core/Platform.h>
#include <Math/Vector3.hpp>

#include <Engine/Engine.hpp>
#include <Engine/Editor.hpp>

#include <Graphics/Skybox.hpp>
#include <Graphics/imgui.h>


static const V3 CUBEMAP_VERTCES[] = {
	V3(-1.0f,  1.0f, -1.0f),
	V3(-1.0f, -1.0f, -1.0f),
	V3(1.0f, -1.0f, -1.0f),
	V3(1.0f, -1.0f, -1.0f),
	V3(1.0f,  1.0f, -1.0f),
	V3(-1.0f,  1.0f, -1.0f),

	V3(-1.0f, -1.0f,  1.0f),
	V3(-1.0f, -1.0f, -1.0f),
	V3(-1.0f,  1.0f, -1.0f),
	V3(-1.0f,  1.0f, -1.0f),
	V3(-1.0f,  1.0f,  1.0f),
	V3(-1.0f, -1.0f,  1.0f),

	V3(1.0f, -1.0f, -1.0f),
	V3(1.0f, -1.0f,  1.0f),
	V3(1.0f,  1.0f,  1.0f),
	V3(1.0f,  1.0f,  1.0f),
	V3(1.0f,  1.0f, -1.0f),
	V3(1.0f, -1.0f, -1.0f),

	V3(-1.0f, -1.0f,  1.0f),
	V3(-1.0f,  1.0f,  1.0f),
	V3(1.0f,  1.0f,  1.0f),
	V3(1.0f,  1.0f,  1.0f),
	V3(1.0f, -1.0f,  1.0f),
	V3(-1.0f, -1.0f,  1.0f),

	V3(-1.0f,  1.0f, -1.0f),
	V3(1.0f,  1.0f, -1.0f),
	V3(1.0f,  1.0f,  1.0f),
	V3(1.0f,  1.0f,  1.0f),
	V3(-1.0f,  1.0f,  1.0f),
	V3(-1.0f,  1.0f, -1.0f),

	V3(-1.0f, -1.0f, -1.0f),
	V3(-1.0f, -1.0f,  1.0f),
	V3(1.0f, -1.0f, -1.0f),
	V3(1.0f, -1.0f, -1.0f),
	V3(-1.0f, -1.0f,  1.0f),
	V3(1.0f, -1.0f,  1.0f)
};


void main() {
	PlatformCreate("CubemapTest");
	Engine::Init();

	ShaderBuilder builder("Skybox");
	builder.addSourceFile(VERTEX_SHADER, SHADER_FILE("skybox.vert"));
	builder.addSourceFile(FRAGMENT_SHADER, SHADER_FILE("skybox.frag"));
	auto& skyboxShaderHandle = LoadShader(builder);

	GLuint vao, vbo;
	CreateSkyboxVertices(&vao, &vbo);


	EditorWindow editorWindow;
	ImGui::Init();

	Camera camera;

	PlatformRun([&](double deltaTime) {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		UpdateCamera(&camera);

		auto& shader = GetShader(skyboxShaderHandle);
		glUseProgram(shader.id);
		SetCameraUniforms(camera);
		DrawSkybox(vao, 0, shader);

		ImGui::BeginFrame();
		ShowEditorWindow(&editorWindow);
		ImGui::EndFrame();
	});
}