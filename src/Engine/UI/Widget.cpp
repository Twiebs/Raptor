#include "Widget.hpp"


Widget::Widget() {
}

Widget::Widget(float width, float height) {
	this->width = width;
	this->height = height;
}

void Widget::SetParent(Widget* widget) {
	this->parent = parent;
}

Widget::~Widget() {

}

void Widget::HandelEvent() {

}