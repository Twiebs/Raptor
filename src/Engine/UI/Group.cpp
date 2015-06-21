#include "Group.hpp"


WidgetGroup::WidgetGroup() {
}


WidgetGroup::~WidgetGroup() {
}

void WidgetGroup::AddWidget(Widget* widget) {
	this->children.push_back(widget);
}
//
//void WidgetGroup::Draw(CanvasRenderer* renderer, float deltaTime) {
//	for each(Widget* widget in children) {
//		widget->Draw(renderer, deltaTime);
//	}
//}


VerticalGroup::VerticalGroup() {

}

VerticalGroup::~VerticalGroup() {

}

void VerticalGroup::Refresh() {
	float currentHeight = y + height;

	for(Widget* widget : children) {
		currentHeight -= widget->GetHeight() + padTop;
		widget->SetPosition(x + padLeft, (currentHeight - widget->GetHeight()));
	}
}
