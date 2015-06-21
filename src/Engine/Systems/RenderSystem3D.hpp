#pragma once

#include <Core/AssetManager.hpp>
#include <Core/Common.hpp>
#include <Core/EntityManager.hpp>
#include <Core/ISystem.hpp>

#include <Components/ModelComponent.hpp>
#include <Components/MeshComponent.hpp>

class RenderSystem3D : public ISystem {
public:
	RenderSystem3D();
	~RenderSystem3D();

	bool Startup(EntityManager* manager) override;
	bool Shutdown(EntityManager* manager) override;
	void Update(double deltaTime) override;

	void ProcessMesh(MeshComponent& mesh, double deltaTime);
	void ProcessModel(ModelComponent& model, double deltaTime);

private:
	uint32 meshComponentID;
	uint32 modelComponentID;

	bool wireframeEnabled;

	AssetID wireframeShaderID;
	AssetID meshShaderID;
};
