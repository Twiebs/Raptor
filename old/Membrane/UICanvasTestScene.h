#pragma once

#include<Core\IScene.h>
#include<UI\Canvas.h>
#include<UI\Font.h>

class UICanvasTestScene : public IScene {
public:
	UICanvasTestScene();
	~UICanvasTestScene();

	void OnLoad(Engine& engine) override;
	void OnDestroy(Engine& engine) override;

	void Tick(float deltaTime) override;
	void Render(float deltaTime) override;

private:
	std::unique_ptr<Canvas> canvas;
	GLuint vertexArrayID;
	GLuint vertexBufferID;
	GLuint elementBufferID;

	GLSLProgram* uiShader;
	Font* font;
	Color* color;
};



