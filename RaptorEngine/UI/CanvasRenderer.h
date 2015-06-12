#pragma once

#include<UI\Font.h>
#include<Graphics\GL\GLSLProgram.h>	
#include<Graphics\Color.h>

#include<glm\matrix.hpp>
#include<glm\gtc\matrix_transform.hpp>

class CanvasRenderer {
public:
	CanvasRenderer();
	~CanvasRenderer();

	void InitGL();

	void RenderFont(Font* font, std::string text, int x, int y, Color* color);

	//Draw functions
	void DrawRect(float x, float y, float width, float height, Color& color);

	void Begin();
	void Flush();

	//Debug render
	void Render();

private:
	GLuint vertexArrayID;
	GLuint vertexBufferID;

	GLSLProgram shader;
	GLSLProgram fontShader;
	glm::mat4 projectionMatrix;
};

