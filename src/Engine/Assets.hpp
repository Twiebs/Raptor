#pragma once
#include <Core/Common.hpp>
#include <GL/glew.h>

#include <Graphics/Render3D.hpp>

// NOTE the extra fielname field is for shaderReloading
//I dont really care about this for now.
//This has the potental to represent an asset file inside a pack
//There fore in the future it is going to need to required an assetTypeID;
//There could also be a userdata field inorder to allow custom types!

namespace Raptor {

struct Asset {
	std::string filename;
};

struct AssetTable {
	std::vector<Asset> modelAssets;
	std::vector<DebugModelData> models;

	std::vector<Asset> shaderAssets;
	std::vector<GLSLProgram> shaderPrograms;
};

void LoadModel(const std::string& filename);
void CreateVertexBuffersForMesh(const MeshData& mesh, VertexBufferGroup& group);

extern AssetTable global_assetTable;

}