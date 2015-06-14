
#include<Application\GLFWApplication.h>
#include<Engine\Engine.h>

#include"TerrainScene.h"
#include"ECSTestScene.h"

int main() {
	GLFWApplication app;
	app.Create("RaptorEngine", 1280, 720, false);

	Engine engine;
	app.Attach(&engine);

	ECSTestScene scene;
	engine.LoadScene(&scene);

	app.Start();
}