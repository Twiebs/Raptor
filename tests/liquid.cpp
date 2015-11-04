#include <Core/Platform.h>

#include <Engine/Editor.hpp>
#include <Engine/Engine.hpp>
#include <Engine/Assets.hpp>
#include <Engine/Debug.hpp>
#include <Engine/GFX3D.hpp>

#include <Graphics/Liquid.hpp>
#include <Graphics/GLRenderer.hpp>
#include <Graphics/GLSLProgram.hpp>
#include <Graphics/Skybox.hpp>
#include <Graphics/imgui.h>

#include <Math/Random.hpp>
#include <Math/Procedural3D.hpp>

void RunDeferredTest() {
	ShaderHandle materialShaderHandle;
	
	MaterialAssetInfo materialInfo = { };
	materialInfo.name = "MyTestMaterial";
	materialInfo.directory = ASSET_FILE("materials/stone_tile/");
	materialInfo.diffuseTextureFilename = "diffuse.tga";
	materialInfo.normalTextureFilename = "normal.tga";
	materialInfo.specularTextureFilename = "specular.tga";
	auto testMaterialHandel = LoadMaterial(materialInfo);

	ProfilerBeginEntry("LoadModels");
	auto rockModelHandle = LoadModel(ASSET_FILE("models/rock0/rock1.obj"));
	auto crateModelHandle = LoadModel(ASSET_FILE("models/containers/wood_barrel_0.obj"));
	ProfilerEndEntry("LoadModels");

	ProfilerBeginEntry("BuildShaders");
	ShaderBuilder materailShaderBuilder("MaterialShader");
	materailShaderBuilder.addSourceFile(VERTEX_SHADER, SHADER_FILE("DeferredGenericMaterial.vert"));
	materailShaderBuilder.addSourceFile(FRAGMENT_SHADER, SHADER_FILE("DeferredGenericMaterial.frag"));
	materialShaderHandle = LoadShader(materailShaderBuilder);

	ShaderBuilder skyboxShaderBuilder("Skybox");
	skyboxShaderBuilder.addSourceFile(VERTEX_SHADER, SHADER_FILE("skybox.vert"));
	skyboxShaderBuilder.addSourceFile(FRAGMENT_SHADER, SHADER_FILE("skybox.frag"));
	auto& skyboxShaderHandle = LoadShader(skyboxShaderBuilder);
	ProfilerEndEntry("BuildShaders");

	GLuint vao, vbo;
	CreateSkyboxVertices(&vao, &vbo);

	// DirectionalLight directionalLight;


	
	// AddLight(directionalLight);


	GLuint vertexArrayID;
	glGenVertexArrays(1, &vertexArrayID);

	int w, h;
	PlatformGetSize(&w, &h);

	Camera camera(w, h, 45.0f, 0.1f, 1000.0f);
	camera.position = V3(32.0f, 6.0f, 32.0f);
	camera.yaw = 0.0f;

	FrameParameters frameParameters = { };
	frameParameters.clearColor = V3(0.1f, 0.1f, 0.1f);

	ImGui::Init();

	EditorWindow editorWindow;
	GFX3D::SetDebugRenderSettings(&editorWindow.renderSettings);

	MeshData data;
	Mesh mesh;

	ProfilerBeginEntry("GeneratePlaneMesh");
	CreatePlaneMesh(&data, 0, 0, 16.0f, 3.1f, 2, 1.0f/16.0f, nullptr);
	ProfilerEndEntry("GeneratePlaneMesh");

	InitMesh(&mesh, &data);
	FreeMeshData(&data);



	Cuboid roomBounds;
	roomBounds.position = V3(0.0);
	roomBounds.size = V3(16.0f);
	Rectangle roomFloor(roomBounds.x, roomBounds.y, roomBounds.w, roomBounds.l);
	

	V3 entityPositions[64];
	Random rng(0);
	for (U32 i = 0; i < 64; i++) {
		auto point = rng.PointInRectangle(roomFloor);
		entityPositions[i] = V3(point.x, 0.0f, point.y);
	}

	PointLight lights[64];
	for (U32 i = 0; i < 16; i++) {
		// lights[i].position = rng.PointInCuboid(roomBounds);
		lights[i].position = V3(0.5f + (1.0f * i), 0.5f, 0.5f);
		lights[i].SetRadius(rng.Range(2.0f, 16.0f));
		lights[i].color = V3(0.35) * rng.vector3();
		AddLight(&lights[i]);
	}


	PlatformRun([&](double deltaTime) -> void {
		int keycode;
		while (keycode = PlatformPopKeypress()) {
			ProcessEditorKeypress(&editorWindow, keycode);
		}

		if (PlatformGetButton(MOUSE_BUTTON_RIGHT)) 
			FPSCameraControlUpdate(&camera);
		if (PlatformGetKey(KEY_F)) {

		}
			// light.position = camera.position + (camera.front * 6.0f);

		GFX3D::BeginFrame(&frameParameters, &camera);

		//auto cubemap = DebugGetShadowMap();
		//auto& skyboxShader = GetShader(skyboxShaderHandle);
		//glUseProgram(skyboxShader.id);
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
		//SetCameraUniforms(camera);
		//auto centeredView = camera.view * Matrix4::Translate(camera.position);
		//Uniform::SetMatrix4(UniformLocation::VIEW_MATRIX, centeredView);
		//DrawSkybox(vao, cubemap, skyboxShader);

		auto& crateModel = GetModel(crateModelHandle);
		auto& rockModel = GetModel(rockModelHandle);
		auto& testMaterial = GetMaterial(testMaterialHandel);
		auto& materialShader = GetShader(materialShaderHandle);


		GFX3D::SetShader(materialShader);
		for (U32 i = 0; i < 64; i++) {
			auto point = rng.PointInRectangle(roomFloor);
			GFX3D::AddModel(crateModel, entityPositions[i]);
		}
		

		GFX3D::SetMaterial((Material*)&testMaterial);
		GFX3D::AddMesh(mesh);
		GFX3D::AddMesh(mesh, Vector3(0, 16.0, 0.0), V3(90.0f, 0.0f, 0.0f));
		GFX3D::AddMesh(mesh, Vector3(16.0, 0.0, 0.0), V3(0.0f, 0.0f, 90.0f));

		GFX3D::EndFrame();

		ImGui::BeginFrame();
		ShowEditorWindow(&editorWindow);
		ImGui::EndFrame();

		Engine::Update();
	});
}


int main() {
	PlatformCreate("GraphicsTestBed", 1792, 1008);
	//CodeModule editorModule = LoadModule("editor.dll");

	Engine::Init();
	RunDeferredTest();
	Engine::Terminate();

	return 0;
}