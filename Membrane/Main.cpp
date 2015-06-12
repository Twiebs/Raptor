
#include<Application\GLFWApplication.h>
#include<Engine\Entity.h>

#include "UICanvasTestScene.h"
#include "SpaceScene.h"
#include "UICanvasTestScene.h"
#include "LODScene.h"


#include<memory>

#include<Math\Random.h>
#include<iostream>
#include<time.h>

int main() {
	GLFWApplication app;
	app.Create("RaptorEngine", 1280, 720, false);

	Engine engine;
	app.Attach(&engine);

	SpaceScene scene;
	engine.LoadScene(&scene);

	app.Start();
}