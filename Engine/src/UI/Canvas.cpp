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

void Canvas::Draw(float deltaTime) {
	for (Widget* child : children) {
		child->Draw(&renderer, deltaTime);
	}
}
