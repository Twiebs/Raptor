#include <Core/Application.hpp>

#include <Graphics/DEBUGRenderer.hpp>
#include <Graphics/Vertex2D.hpp>

bool gRunning = true;

DEBUGBufferGroup gBufferGroup;

//TODO KeyCallback access to app?
void KeyCallback(int button, bool isDown) {
	switch (button) {
	case KEY_ESCAPE:
		gRunning = false;
		break;
	}
}

void MainLoop(Application* app) {
	app->isRunning = gRunning;

	DEBUGBindGroup(&gBufferGroup);

}

int main() {
	Application app;
	app.Create("2DStarRenderer", 1280, 720, false);

	DEBUGCreateBufferGroup(&gBufferGroup, sizeof(Vertex2D), 4096);
	DEBUGAddAttribute(&gBufferGroup, 0, 2, GL_FLOAT, sizeof(Vertex2D), (void*)offsetof(Vertex2D, position));
	DEBUGAddAttribute(&gBufferGroup, 0, 2, GL_FLOAT, sizeof(Vertex2D), (void*)offsetof(Vertex2D, uv));
	DEBUGAddAttribute(&gBufferGroup, 0, 4, GL_FLOAT, sizeof(Vertex2D), (void*)offsetof(Vertex2D, color));

	app.SetKeyCallback(KeyCallback);
	app.Run(MainLoop, &app);
	app.Destroy();
	return 0;
}