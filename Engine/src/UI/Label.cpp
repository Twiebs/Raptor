#include "Label.h"

Label::Label() {

}

Label::Label(std::string text, Font* font, Color color) {
	Init(text, font, color);
}

void Label::Init(std::string text, Font* font, Color color) {
	this->text = text;
	this->font = font;
	this->color = color;
}


Label::~Label() {
}

void Label::Draw (CanvasRenderer* renderer, float deltaTime) {
	renderer->RenderFont(font, text, GetX(), GetY(), &color);
}