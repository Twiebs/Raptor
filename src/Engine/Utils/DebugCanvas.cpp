// #include "DebugCanvas.hpp"
//
// #include <sstream>
//
// #include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
// #include <glm/gtc/type_ptr.hpp>
//
// DebugCanvas::DebugCanvas(float width, float height) : Canvas(width, height) {
// 	Setup();
// }
//
// DebugCanvas::~DebugCanvas() {
// }
//
// void DebugCanvas::Setup() {
//
// 	rotationLabel.Init("Rotation: [XX, XX]", font, Color(1.0f, 1.0f, 1.0f, 1.0f));
// 	positionLabel.Init("Position: [XX, XX, XX]", font, Color(1.0f, 1.0f, 1.0f, 1.0f));
// 	fpsLabel.Init("FPS: XXX", font, Color(1.0f, 1.0f, 0.0f, 1.0f));
// 	vertLabel.Init("Verts: XXXX", font, Color());
//
// 	group.SetBounds(10, 620, 250, 100);
// 	group.AddWidget(&fpsLabel);
// 	group.AddWidget(&positionLabel);
// 	group.AddWidget(&rotationLabel);
// 	group.AddWidget(&vertLabel);
// 	group.Refresh();
// 	AddWidget(&group);
// }
//
// void DebugCanvas::Update(CommonScene* scene, float deltaTime){
// 	Update(deltaTime);
//
// #ifdef DEBUG
// 	vertLabel.SetText("Verts: " + std::to_string(scene->renderer->vertexCount));
// 	scene->renderer->vertexCount = 0;
// #endif // DEBUG
// }
// 
// void DebugCanvas::Update(float deltaTime) {
// 	std::stringstream stream;
// 	if (camera != nullptr) {
// 		stream << "Position: " << camera->position;
// 		positionLabel.SetText(stream.str());
//
// 		stream.str("");
// 		stream << "Yaw: " << camera->yaw << " Pitch: " << camera->pitch;
// 		rotationLabel.SetText(stream.str());
// 	}
//
// 	fpsLabel.SetText("FPS: " + std::to_string((int)std::round(1.0 / deltaTime)));
// }
