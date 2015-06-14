#pragma once

#include<vector>
#include "Widget.h"

class WidgetGroup : public Widget{
public:
	WidgetGroup();
	~WidgetGroup();

	void Draw(CanvasRenderer* renderer, float deltaTime) override;

	void AddWidget(Widget* widget);
	virtual void Refresh() = 0;

protected:
	std::vector<Widget*> children;

	float padLeft;
	float padTop = 4;
};

class VerticalGroup : public WidgetGroup {
public:
	VerticalGroup();
	~VerticalGroup();

	void Refresh() override;
};
