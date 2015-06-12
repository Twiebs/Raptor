
#include <Application\GLFWApplication.h>
#include <Engine\Engine.h>

#include "NoiseScene.h"
#include "ModelViewerScene.h"

GLFWApplication app;
Engine engine;

int main(){
	app.Create("RaptorTools", 1280, 720, false);
	app.Attach(&engine);

	ModelViewerScene scene;
	engine.LoadScene(&scene);

	app.Start();
	return 0;
}