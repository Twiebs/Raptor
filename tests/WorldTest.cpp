#include <Core/Platform.h>

#include <Math/Random.hpp>

#include <Graphics/Render3D.hpp>
#include <Graphics/imgui.h>

#include <Engine/Assets.hpp>
#include <Engine/World.hpp>
#include <Engine/Editor.hpp>

using namespace Raptor;

#if 0
//#include <btBulletDynamicsCommon.h>
//struct PhysicsSim {
//	btDbvtBroadphase broadphase;
//	btDefaultCollisionConfiguration collisionConfig;
//	btCollisionDispatcher dispatcher;
//	btSequentialImpulseConstraintSolver solver;
//	btDiscreteDynamicsWorld dynamicsWorld;
//};
//
//void InitPhysics() {
//	btBroadphaseInterface* broadphase = new btDbvtBroadphase();
//	btDefaultCollisionConfiguration* collisionConfig = new btDefaultCollisionConfiguration();
//	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfig);
//	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
//
//	btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfig);
//	dynamicsWorld->setGravity(btVector3(0, -10, 0));
//}

#endif

global_variable World global_world;
global_variable Editor global_editor;
global_variable DeferredShader global_DeferredShader;

void RenderTest (Camera& camera) {
	static GLuint basicShader = CreateShader("shaders/Basic.vert", "shaders/Basic.frag");
	glUseProgram(basicShader);
	//auto& model = global_assetTable.models[1];
	Matrix4 modelMatrix = Matrix4::Identity();
	PushMatrix(MODEL_LOCATION, modelMatrix);
	PushMatrix(VIEW_LOCATION, camera.view);
	PushMatrix(PROJECTION_LOCATION, camera.projection);
	for (auto i = 0; i < 64; i++) {
		modelMatrix = Matrix4::Translate(i * 1.5f, 0.0f, i * 1.5f);
		PushMatrix(MODEL_LOCATION, modelMatrix);
	//	Draw(model);
	}
}

void OutlineSelected(World& world, Editor& editor, Camera& camera) {
	if (editor.selectedCount < 1) return;
	glDisable(GL_DEPTH_TEST);

	for (auto i = 0; i < 64; i++) {
		if (editor.isSelected[i]) {
			U32* modelIDs = (U32*)world.components[COMPONENT_MODELID];
			auto transforms = (Transform3D*)world.components[COMPONENT_TRANSFORM];
			auto& transform = transforms[i];
			//auto& model = global_assetTable.models[modelIDs[i]];

			static GLuint basicShader = CreateShader("shaders/Basic.vert", "shaders/Basic.frag");
			glUseProgram(basicShader);

			Matrix4 modelMatrix = TransformToMatrix(transform);
			PushMatrix(MODEL_LOCATION, modelMatrix);
			PushMatrix(VIEW_LOCATION, camera.view);
			PushMatrix(PROJECTION_LOCATION, camera.projection);
		//	Draw(model);
		}
	}
	glEnable(GL_DEPTH_TEST);
}

void MouseCast(Camera& camera) {
	int cursorX, cursorY;
	int width, height;
	PlatformGetCursorPos(&cursorX, &cursorY);

	float x = (2.0f * cursorX) / ((float)width - 1.0f);
	float y = 1.0f - (2.0f * cursorY) / (float)height;
	Vector4 rayClip(x, y, -1.0, 1.0);
}

void Raycast(Vector3& origin);


// Dont do this anymore!
// Lets make the app only functions as it should have originaly been!
void MainLoop(F64 deltaTime) {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (PlatformGetKey(KEY_ESCAPE)) {
		memset(global_editor.isSelected, 0, 64);
		global_editor.selectedCount = 0;
	}

	if (PlatformGetKey(KEY_Q) && PlatformGetKey(KEY_LCTRL)) PlatformExit();

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	//app->SetCursorHidden(false);
	if (PlatformGetButton(MOUSE_BUTTON_RIGHT)) {
		//app->SetCursorHidden(true);
		FPSCameraControlUpdate(&global_editor.camera);
	}

	UpdateCamera(&global_editor.camera);

	UpdateWorld(global_world);
	RenderWorld(global_world, global_DeferredShader, global_editor.camera);

	OutlineSelected(global_world, global_editor, global_editor.camera);

	ImGui::BeginFrame();
	ShowWorldEditor(global_world, global_editor);
	ImGui::EndFrame();

	//ImGui::BeginFrame();
	//ImGui::Begin("Camera");
	//ImGui::Text("Position: [%f, %f, %f]", global_editor.camera.position.x, global_editor.camera.position.y, global_editor.camera.position.z);
	//ImGui::Text("Pitch: %f", global_editor.camera.pitch);
	//ImGui::Text("Yaw: %f", global_editor.camera.yaw);
	//ImGui::End();

	//static bool lastState = true;
	//static bool showEditor = false;
	//auto keydown = PlatformGetKey(KEY_F1);
	//if (keydown && lastState != showEditor) {
	//	lastState = showEditor;
	//	showEditor = !showEditor;
	//}

	//ImGui::SetNextWindowCollapsed(!showEditor);
	//ShowWorldEditor(global_world, global_editor);
	//ImGui::EndFrame();
}

int main() {
	int x = 5;

	PlatformCreate("World Test");

	LoadWorld(global_world, "editor/test.world");
	LoadEditor(global_editor, "editor/test.editor");
	Assets::LoadModel("models/containers/crate.obj");
	Assets::LoadModel("models/architecture/pillar.obj");

	int width, height;
	PlatformGetSize(&width, &height);
	global_editor.camera.position = Vector3(128.0f, 2.0f, 128.0f);
	global_editor.camera.viewportWidth = width;
	global_editor.camera.viewportHeight = height;
	global_editor.camera.nearClip = 0.1f;
	global_editor.camera.farClip = 1000.0f;

	ImGui::Init();

	InitDeferredShader(&global_DeferredShader, width, height);
	glUseProgram(global_DeferredShader.lightingPassProgram);
	glUniform1i(GetUniformLocation(global_DeferredShader.lightingPassProgram, "pointLightCount"), global_world.pointLightCount);

	PlatformRun(MainLoop);
	return 0;
}
