#pragma once

#include<UI\CanvasRenderer.h>
#include<Graphics\Color.h>

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

	void SetPosition(float x, float y) { 
		this->x = x; 
		this->y = y; 
	}

	void SetBounds(float x, float y, float width, float height) {
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
	}
	void SetParent(Widget* widget);

	virtual void Draw (CanvasRenderer* renderer, float deltaTime) = 0;
	virtual void HandelEvent();

protected:
	float x, y;
	float width, height;
	Widget* parent;

	Color color;
private:

};

