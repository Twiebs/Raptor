#pragma once

#include<string>

#include<Graphics\Camera.h>
#include<Graphics\Shader.h>

#include<UI\Canvas.h>
#include<UI\Font.h>
#include<UI\Label.h>
#include<UI\Widget.h>

class DebugCanvas : public Canvas{
public:
	Camera* camera;

	DebugCanvas(float width, float height);
	~DebugCanvas();

	void Setup() override;
	void Update(float deltaTime) override;

private:
	Font* font;

	Label positionLabel;
	Label rotationLabel;
	Label fpsLabel;
};

