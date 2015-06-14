#pragma once

#include<vector>

#include<Core\Input.h>
#include<UI\Widget.h>

class Canvas : IInputListener{
public:
	Canvas(int screenWidth, int screenHeight);
	~Canvas();

	float GetViewportWidth() { return viewportWidth; }
	float GetViewportHeight() { return viewportHeight; }

	void AddWidget(Widget* widget);

	virtual void Setup();
	virtual void Update(float deltaTime);
	virtual void Render(float deltaTime);
	 
protected:
	int viewportWidth, viewportHeight;
	std::vector<Widget*> children;

	CanvasRenderer renderer;

	bool OnMouseButtonEvent(int button, bool isPressed, int mods) override;
};

