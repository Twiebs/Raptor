#include "Canvas.h"


Canvas::Canvas(int viewportWidth, int viewportHeight) {
	this->viewportWidth = viewportWidth;
	this->viewportHeight = viewportHeight;
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

bool Canvas::OnMouseButtonEvent(int button, bool isPressed, int mods){
	for (Widget* widget : children){
		widget->HandelEvent();
	}
	return false;
}