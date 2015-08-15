
#include <Core/Application.hpp>
#include <Graphics/Render3D.hpp>

using namespace Raptor;

//Dont do this anymore!
//Lets make the app only functions as it should have originaly been!
void MainLoop(Application* app) {
	if (app->IsKeyDown(KEY_ESCAPE)) app->isRunning = false;

	static Camera camera(Vector3(10.0f, 2.0f, 15.0f), app->GetWidth(), app->GetHeight());
	static GLuint debugLightShader = LoadShaderFromFile("shaders/DebugLights.vert", "shaders/DebugLights.frag");
	static Vector3 cubePos = Vector3(10, 2, 10);

	FPSCameraControlUpdate(app, camera);
	UpdateCamera(camera);
	Matrix4 model = Matrix4::Translate(cubePos);
	Matrix4 mvp = camera.projection * camera.view * model;

	glUseProgram(debugLightShader);
	glUniformMatrix4fv(GetUniformLocation(debugLightShader, "mvp"), 1, GL_FALSE, &mvp[0][0]);

	DrawDebugCube();
}


#include <btBulletDynamicsCommon.h>
struct PhysicsSim {
	btDbvtBroadphase broadphase;
	btDefaultCollisionConfiguration collisionConfig;
	btCollisionDispatcher dispatcher;
	btSequentialImpulseConstraintSolver solver;
	btDiscreteDynamicsWorld dynamicsWorld;
};

void InitPhysics() {
	btBroadphaseInterface* broadphase = new btDbvtBroadphase();
	btDefaultCollisionConfiguration* collisionConfig = new btDefaultCollisionConfiguration();
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfig);
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

	btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfig);
	dynamicsWorld->setGravity(btVector3(0, -10, 0));
}

int main() {
	Application app("WorldTest");
	app.Run(MainLoop);
	return 0;
}