#pragma once

#include<UI\Font.h>
#include<Graphics\Shader.h>	
#include<UI\Color.h>

#include<glm\matrix.hpp>
#include<glm\gtc\matrix_transform.hpp>

class CanvasRenderer {
public:
	CanvasRenderer();
	~CanvasRenderer();

	void RenderFont(Font* font, std::string text, int x, int y, Color* color);

private:
	ShaderProgram fontShader;
	glm::mat4 projectionMatrix;
};

