#include "Canvas.hpp"


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

	//for (Widget* child : children) {
	//	child->Draw(&renderer, deltaTime);
	//}
	//renderer.Flush();
}

bool Canvas::OnMouseButtonEvent(int button, double xpos, double ypos, bool isPressed, int mods){
	for (Widget* widget : children){
		widget->HandelEvent();
	}
	return false;
}