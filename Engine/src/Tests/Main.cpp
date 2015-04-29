
#include<Application\GLFWApplication.h>

#include"UICanvasTestScene.h"
#include"SpaceScene.h"

int main() {
	GLFWApplication app;
	app.Create("RaptorEngine", 1280, 720, false);

	IScene* scene = new SpaceScene();
	app.engine->LoadScene(scene);
	app.Start();
}