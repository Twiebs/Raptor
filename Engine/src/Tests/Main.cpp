
#include<Application\GLFWApplication.h>
#include<Core\Engine.h>

#include"UICanvasTestScene.h"
#include"SpaceScene.h"
#include"SkyboxTestScene.h"
#include"UICanvasTestScene.h"

#include<memory>

int main() {
	GLFWApplication app;
	app.Create("RaptorEngine", 1280, 720, false);

	Engine engine;
	app.Attach(&engine);

	SpaceScene scene;
	engine.LoadScene(&scene);

	app.Start();
}