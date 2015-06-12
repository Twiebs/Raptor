#pragma once

#include<Graphics\Camera.h>
#include<Graphics\Model.h>
#include<Graphics\Mesh.h>
#include<Graphics\Skybox.h>

//Renderer interface for harware accellerated technologies(OpenGL, DX, ...) to implement
class IRenderer {
public:
	//Public debug vars

#ifdef DEBUG
	int vertexCount;
#endif // DEBUG


	IRenderer(Camera* camera);
	~IRenderer();

	void SetCamera(Camera* camera);

	//Draw all the resources you could possibly want...
	virtual void Draw(std::unique_ptr<Model>& model, std::unique_ptr<GLSLProgram>& shader) = 0;
	virtual void Draw(std::unique_ptr<Mesh>& mesh, std::unique_ptr<GLSLProgram>& shader) = 0;
	virtual void Draw(Mesh* mesh, std::unique_ptr<GLSLProgram>& shader) = 0;
	virtual void Draw(std::unique_ptr<Skybox>& skybox, std::unique_ptr<GLSLProgram>& shader) = 0;

	//Debug stuff
	virtual void SetWireframeEnabled(bool enabled) = 0;
	void ToggleWireframeEnabled();
protected:
	Camera* camera;

	//Debug
	bool wireframeEnabled;

};

