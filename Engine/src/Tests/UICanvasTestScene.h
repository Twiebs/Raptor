#pragma once

#include<Core\Scene.h>
#include<UI\Font.h>

class UICanvasTestScene : public BaseScene{
public:
	UICanvasTestScene();
	~UICanvasTestScene();

	void OnLoad(Engine* engine) override;
	void OnDestroy(Engine* engine) override;

	void Tick(float deltaTime) override;
	void Render(float deltaTime) override;

private:
	ShaderProgram* shader;
	Font* font;

	Color* color;
};



