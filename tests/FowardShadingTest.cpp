#include <Core/Platform.h>
#include <Engine/GFX3D.hpp>
#include <Graphics/Render3D.hpp>

using namespace Raptor;
DebugModelData model;

FowardShader fowardShader;
DepthShader depthShader;

Camera camera;

PointLight light(Vector3(1.0f, 1.0f, 1.0f));

void MainLoop(F64 deltaTime) {
	UpdateCamera(&camera);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	BeginFowardShadingPass(&fowardShader, &depthShader, &camera);
	PushLight(light, 0, fowardShader.program);
	Draw(model);
	EndFowardShadingPass();
}

int main() {
	PlatformCreate("FowardShadingTest");
	InitFowardShader(&fowardShader);
	InitDepthShader(&depthShader, 1024, 1024);
	model.ImportFromFile("assets/models/containers/crate.obj");

	camera = Camera(Vector3(0.0f, 0.0f, -10.0f), 1280.0f, 720.0f);

	PlatformRun(MainLoop);
}