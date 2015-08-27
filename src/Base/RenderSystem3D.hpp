#pragma once

#include <Core/Common.hpp>
#include <ECS/EntityManager.hpp>
#include <ECS/ISystem.hpp>

#include <Base/ModelComponent.hpp>
#include <Base/MeshComponent.hpp>

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
