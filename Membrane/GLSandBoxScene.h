#pragma once
#include <Utils\CommonScene.h>
#include <Graphics\Model.h>

class GLSandBoxScene : public CommonScene{
public:
	GLSandBoxScene();
	~GLSandBoxScene();

	void OnLoad(Engine& engine) override;

	void Render(float deltaTime) override;
private:
	std::unique_ptr<Model> nanosuitModel;
	std::unique_ptr<GLSLProgram> modelShader;
};

