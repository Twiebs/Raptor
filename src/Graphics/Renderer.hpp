#pragma once

#include <Math/Vector3.hpp>
#include <Graphics/Camera.hpp>
#include <Graphics/GLSLProgram.hpp>
#include <Graphics/Mesh.hpp>
#include <Graphics/Model.hpp>
#include <Engine/Terrain.hpp>

struct FrameParameters {
	V3 clearColor;
};


class Renderer {
public:
	Renderer();
	~Renderer();

	void NewFrame(const FrameParameters& params, Camera* camera);
	void RenderScene();

	void SetMaterialShader(Shader* shader);

	void AddMesh(Mesh* mesh, const V3& position = V3(0.0f), const V3& rotation = V3(0.0f), const V3& scale = V3(1.0f));
	void AddModel(Model* model, const V3& position = V3(0.0f));
	void AddTerrain(TerrainManager* terrain);

private:
	Camera* currentCamera;
	Shader* currentMaterialShader;
};

