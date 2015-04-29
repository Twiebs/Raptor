#include "DebugCanvas.h"

#include<sstream>

#include<glm\glm.hpp>
#include<glm\gtc\matrix_transform.hpp>
#include<glm\gtc\type_ptr.hpp>

DebugCanvas::DebugCanvas(float width, float height) : Canvas(width, height) {
	Setup();
}

DebugCanvas::~DebugCanvas() {
}

void DebugCanvas::Setup() {
	font = new Font("Resources/fonts/arial.ttf", 18);

	rotationLabel.Init("Rotation: [XX, XX]", font, Color(1.0f, 1.0f, 1.0f));
	rotationLabel.SetPosition(20, GetViewportHeight() - 55);

	positionLabel.Init("Position: [XX, XX, XX]", font, Color(1.0f, 1.0f, 1.0f));
	positionLabel.SetPosition(20, GetViewportHeight() - 20);

	fpsLabel.Init("FPS: XXX", font, Color(1.0f, 1.0f, 0.0f));
	fpsLabel.SetPosition(GetViewportWidth() - 60, GetViewportHeight() - 20);

	AddWidget(&fpsLabel);
	AddWidget(&positionLabel);
	AddWidget(&rotationLabel);
}

void DebugCanvas::Update(float deltaTime) {
	std::stringstream stream;
	if (camera != nullptr) {
		stream << "Position: " << camera->position;
		positionLabel.SetText(stream.str());

		stream.str("");
		stream << "Yaw: " << camera->yaw << " Pitch: " << camera->pitch;
		rotationLabel.SetText(stream.str());
	}

	fpsLabel.SetText("FPS: " + std::to_string((int)std::round(1.0 / deltaTime)));
}
