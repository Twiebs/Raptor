#pragma once

#include<sstream>

#include<Graphics\IRenderer.h>

class GLRenderer : public IRenderer{
public:
	GLRenderer(Camera* camera);
	~GLRenderer();

	void Draw(std::unique_ptr<Model>& model, std::unique_ptr<GLSLProgram>& shader) override;
	void Draw(std::unique_ptr<Mesh>& mesh, std::unique_ptr<GLSLProgram>& shader) override;
	void Draw(Mesh* mesh, std::unique_ptr<GLSLProgram>& shader) override;
	void Draw(std::unique_ptr<Skybox>& skybox, std::unique_ptr<GLSLProgram>& shader) override;

	void DrawWireframe(std::unique_ptr<Mesh>& mesh);
	void DrawWireframe(Mesh* mesh);

	void SetWireframeEnabled(bool enabled) override;
	void SetTransformMatrix(glm::mat4& transform);

private:
	glm::mat4 transform;
	//Debug
	std::unique_ptr<GLSLProgram> wireframeShader;
};

