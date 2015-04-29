#pragma once

#include<vector>

#include<UI\Widget.h>

class Canvas {
public:
	Canvas(int screenWidth, int screenHeight);
	~Canvas();

	float GetViewportWidth() { return screenWidth; }
	float GetViewportHeight() { return screenHeight; }

	void AddWidget(Widget* widget);

	virtual void Setup();
	virtual void Update(float deltaTime);
	virtual void Draw(float deltaTime);

private:
	CanvasRenderer renderer;
	int screenWidth, screenHeight;

	std::vector<Widget*> children;
};

