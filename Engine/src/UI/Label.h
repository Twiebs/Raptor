#pragma once

#include<string>

#include<Graphics\Shader.h>

#include<UI\Font.h>
#include<UI\Widget.h>

class Label : public Widget{
public:
	Label();
	Label(std::string text, Font* font, Color color);
	~Label();

	void Init(std::string text, Font* font, Color color);
	void SetText(std::string text);
	std::string GetText() { return text; }
	void Draw (CanvasRenderer* renderer, float deltaTime) override;

private:
	std::string text;
	Font* font;
};

