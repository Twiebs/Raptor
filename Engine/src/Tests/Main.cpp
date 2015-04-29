
#include<Application\GLFWApplication.h>
#include<Core\Engine.h>

#include"UICanvasTestScene.h"
#include"SpaceScene.h"

int main() {
	GLFWApplication app;
	app.Create("RaptorEngine", 1280, 720, false);

	Engine* engine = new Engine()
	IScene* scene = new SpaceScene();
	engine->LoadScene(scene);

	app.Start(engine);
}