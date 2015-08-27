
#include <Core/Application.hpp>
#include <Core/Platform.h>

#include <Graphics/Render3D.hpp>
#include <Graphics/GUI.hpp>

#include <Math/Random.hpp>

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
global_variable GUIContext global_GUIContext;

void RenderTest(Camera& camera) {
	static GLuint basicShader = LoadShaderFromFile("shaders/Basic.vert", "shaders/Basic.frag");
	glUseProgram(basicShader);
	auto& model = global_assetTable.models[1];
	Matrix4 modelMatrix = Matrix4::Identity();
	PushMatrix(MODEL_LOCATION, modelMatrix);
	PushMatrix(VIEW_LOCATION, camera.view);
	PushMatrix(PROJECTION_LOCATION, camera.projection);
	for (auto i = 0; i < 64; i++) {
		modelMatrix = Matrix4::Translate(i * 1.5f, 0.0f, i * 1.5f);
		PushMatrix(MODEL_LOCATION, modelMatrix);
		Draw(model);
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
			auto& model = global_assetTable.models[modelIDs[i]];

			static GLuint basicShader = LoadShaderFromFile("shaders/Basic.vert", "shaders/Basic.frag");
			glUseProgram(basicShader);

			Matrix4 modelMatrix = TransformToMatrix(transform);
			PushMatrix(MODEL_LOCATION, modelMatrix);
			PushMatrix(VIEW_LOCATION, camera.view);
			PushMatrix(PROJECTION_LOCATION, camera.projection);
			Draw(model);
		}
	}
	glEnable(GL_DEPTH_TEST);
}

void MouseCast(Camera& camera) {
	int cursorX, cursorY;
	int width, height;
	PlatformGetCursorPos(&cursorX, &cursorY);
	GetWindowSize(&width, &height);

	float x = (2.0f * cursorX) / ((float)width - 1.0f);
	float y = 1.0f - (2.0f * cursorY) / (float)height;
	Vector4 rayClip(x, y, -1.0, 1.0);
}

void Raycast(Vector3& origin);


// Dont do this anymore!
// Lets make the app only functions as it should have originaly been!
void MainLoop(Application* app) {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (app->IsKeyDown(KEY_ESCAPE)) {
		memset(global_editor.isSelected, 0, 64);
		global_editor.selectedCount = 0;
	}


	if (PlatformGetKey(KEY_Q) && PlatformGetKey(KEY_LCTRL)) app->isRunning = false;

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	app->SetCursorHidden(false);
	if (app->IsButtonDown(MOUSE_BUTTON_RIGHT)) {
		app->SetCursorHidden(true);
		FPSCameraControlUpdate(app, global_editor.camera);
	}

	UpdateCamera(global_editor.camera);

	UpdateWorld(global_world);
	RenderWorld(global_world, global_DeferredShader, global_editor.camera);

	OutlineSelected(global_world, global_editor, global_editor.camera);
	//RenderTest(global_editor.camera);

	GUIBeginFrame(&global_GUIContext, app);
	static bool lastState = true;
	static bool showEditor = false;
	auto keydown = PlatformGetKey(KEY_F1);	
	if (keydown && lastState != showEditor) {
		lastState = showEditor;
		showEditor = !showEditor;
	}

	ImGui::SetNextWindowCollapsed(!showEditor);
	ShowWorldEditor(global_world, global_editor);
	GUIEndFrame();
}

int main() {
	Application app("WorldTest", 1920, 1080);

	LoadWorld(global_world, "test.world");
	LoadEditor(global_editor, "test.editor");
	LoadModel("models/containers/crate.obj");
	LoadModel("models/Pillars/pillar/pillar.obj");

	global_editor.camera.position = Vector3(128.0f, 2.0f, 128.0f);
	global_editor.camera.viewportWidth = app.GetWidth();
	global_editor.camera.viewportHeight = app.GetHeight();
	global_editor.camera.nearClip = 0.1f;
	global_editor.camera.farClip = 1000.0f;

	int width, height;
	GUIContextInit(&global_GUIContext, app.GetWidth(), app.GetHeight());

	InitDeferredShader(global_DeferredShader, app.GetWidth(), app.GetHeight());
	glUseProgram(global_DeferredShader.lightingPassProgram);
	glUniform1i(GetUniformLocation(global_DeferredShader.lightingPassProgram, "pointLightCount"), global_world.pointLightCount);

	app.Run(MainLoop);
	return 0;
}