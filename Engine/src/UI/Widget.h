#pragma once

#include<UI\CanvasRenderer.h>
#include<UI\Color.h>

class Widget {
public:
	Widget();
	Widget(float width, float height);
	~Widget();

	float GetX() { return x; }
	float GetY() { return y; }
	float GetWidth() { return width; }
	float GetHeight() { return height; }

	Color GetColor() { return color; }

	void SetPosition(float x, float y) { this->x = x; this->y = y; }
	void SetParent(Widget* widget);

	virtual void Draw (CanvasRenderer* renderer, float deltaTime) = 0;

protected:
	float x, y;
	float width, height;
	Widget* parent;

	Color color;
private:

};

