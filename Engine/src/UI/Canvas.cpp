#include "Canvas.h"


Canvas::Canvas(int screenWidth, int screenHeight) {
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
	this->Setup();
}


Canvas::~Canvas() {
}

void Canvas::Setup() {

}

void Canvas::AddWidget(Widget* widget) {
	this->children.push_back(widget);
}

void Canvas::Update(float deltaTime) {

}

void Canvas::Render(float deltaTime) {
	renderer.Begin();
	renderer.DrawRect(10, 10, 200, 20, Color(1.0f, 1.0f, 1.0f, 1.0f));
	renderer.DrawRect(500, 10, 200, 20, Color(1.0f, 0.0f, 0.0f, 0.25f));

	for (Widget* child : children) {
		child->Draw(&renderer, deltaTime);
	}
	renderer.Flush();
}
