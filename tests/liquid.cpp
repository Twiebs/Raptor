#include <Core/Platform.h>

#include <Engine/Assets.hpp>
#include <Engine/Debug.hpp>
#include <Engine/GFX3D.hpp>

#include <Math/Procedural3D.hpp>

#include <Graphics/Liquid.hpp>
#include <Graphics/GLRenderer.hpp>
#include <Graphics/GLSLProgram.hpp>
#include <Graphics/imgui.h>

static void ControlCamera(const Camera& camera) {
	if (PlatformGetButton(MOUSE_BUTTON_RIGHT)) FPSCameraControlUpdate((Camera*)&camera);
}

void RunLiquidTest() {
	LiquidRenderParameters liquidSettings;
	liquidSettings.amplitude = 1.0f;
	liquidSettings.period = 0.5f;
	liquidSettings.wavelength = 2.0f;

	BeginShaderBuilder("water_shader");
	AddShaderSourceFile(VERTEX_SHADER, SHADER_FILE("liquid.vert"));
	AddShaderSourceFile(FRAGMENT_SHADER, SHADER_FILE("liquid.frag"));
	auto liquid_shader = _builder.build_program();

	ShaderBuilder builder("debug");
	builder.addSourceFile(VERTEX_SHADER, SHADER_FILE("debug.vert"));
	builder.addSourceFile(FRAGMENT_SHADER, SHADER_FILE("debug.frag"));
	auto debugShader = builder.build_program();

	BindShader(liquid_shader);
	SetLiquidRenderSettingsUniforms(&liquidSettings);

	MeshData data;
	Mesh mesh;
	CreatePlaneMesh(&data, 0, 0, 32, 32, 32, 1);
	InitMesh(&mesh, &data);

	Camera camera(Vector3(0.0f, 0.0f, -10.0f), 1280, 720);

	ImGui::Init();

	PlatformRun([&](double deltaTime) -> void {
		ControlCamera(camera);

		GFX3D::BeginFrame(camera);
		GFX3D::Begin(liquid_shader);
		GFX3D::DrawMesh(mesh);
		GFX3D::End();
		GFX3D::EndFrame();

		ImGui::BeginFrame();
		ShowCameraDebugInfo(camera);
		ImGui::EndFrame();

	});
}

int main() {
	PlatformCreate("GraphicsTestBed");
	RunLiquidTest();
	return 0;
}