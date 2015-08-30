#include <Core/Platform.h>

#include <Graphics/ModelData.hpp>
#include <Graphics/Texture.hpp>

#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/mesh.h>

using namespace Raptor;

struct ImportedMesh {
	U32 vertexCount, indexCount;
	Vertex3D* vertices;
	U32* indices;
	U8* memblock;
};

struct ImportedModel {
	std::vector<ImportedMesh> meshes;
	std::vector<U32> meshMaterialIndex;
	std::vector<VertexBufferGroup> meshVertexBuffers;
	std::vector<Material> materials;
};

struct ModelData {
	U32 meshCount;
	U32* meshOffsets;
	Vertex3D* vertices;
	U32* indices;
	U8* memory;
};

void ImportModelData(ModelData* data, const std::string& filename) {
	Assimp::Importer importer;
	auto scene = importer.ReadFile(ASSET_DIR + filename, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenNormals);
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		LOG_ERROR("Assimp failed to load file (" << filename << ") " << importer.GetErrorString());
		return;
	}
	
	U32 vertexCount = 0;
	U32 indexCount = 0;
	for (auto i = 0; i < scene->mNumMeshes; i++) {
		vertexCount += scene->mMeshes[i]->mNumVertices;
		indexCount += scene->mMeshes[i]->mNumFaces * 3;
	}

	auto vertexMemsize = vertexCount * sizeof(Vertex3D);
	auto indexMemsize = indexCount * sizeof(U32);
	auto meshOffsetMemsize = scene->mNumMeshes * sizeof(U32);
	data->memory = (U8*)malloc(meshOffsetMemsize + vertexMemsize + indexMemsize);
	data->vertices = (Vertex3D*)data->memory;
	data->indices = (U32*)(data->vertices + vertexMemsize);
	data->meshOffsets = (U32*)(data->indices + indexMemsize);
	for (auto i = 0; i < scene->mNumMeshes; i++) {
		auto ai_mesh = scene->mMeshes[i];
		for (auto i = 0; i < ai_mesh->mNumVertices; i++) {
			data->vertices[i].position.x = ai_mesh->mVertices[i].x;
			data->vertices[i].position.y = ai_mesh->mVertices[i].y;
			data->vertices[i].position.z = ai_mesh->mVertices[i].z;
			data->vertices[i].normal.x = ai_mesh->mNormals[i].x;
			data->vertices[i].normal.y = ai_mesh->mNormals[i].y;
			data->vertices[i].normal.z = ai_mesh->mNormals[i].z;
			data->vertices[i].tangent.x = ai_mesh->mTangents[i].x;
			data->vertices[i].tangent.y = ai_mesh->mTangents[i].x;
			data->vertices[i].tangent.z = ai_mesh->mTangents[i].x;
			if (ai_mesh->mTextureCoords[0]) {
				data->vertices[i].texCoord.x = ai_mesh->mTextureCoords[0][i].x;
				data->vertices[i].texCoord.y = ai_mesh->mTextureCoords[0][i].y;
			}
		}
		for (auto index = 0, i = 0; i < ai_mesh->mNumFaces * 3; index++, i += 3) {
			data->indices[i + 0] = ai_mesh->mFaces[index].mIndices[0];
			data->indices[i + 1] = ai_mesh->mFaces[index].mIndices[1];
			data->indices[i + 2] = ai_mesh->mFaces[index].mIndices[2];
		}
	}

	auto directory = filename.substr(0, filename.find_last_of("/") + 1);
	model->materials.resize(scene->mNumMaterials);

	for (auto i = 0; i < scene->mNumMaterials; i++) {
		auto material = scene->mMaterials[i];

		auto diffuseTextureCount = material->GetTextureCount(aiTextureType_DIFFUSE);
		if (diffuseTextureCount > 0) {
			aiString texturePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath);
			if (strcmp(texturePath.C_Str(), "")) {
				model->materials[i].diffuseMapID = CreateTextureFromFile(directory + texturePath.C_Str());
			} else {
				LOG_WARNING("Diffuse Texure for: " << filename << " has an invalid filename");
			}
		} else {
			LOG_WARNING("Model: " << filename << " does not have a diffuse texture!");
		}

		auto specularTextureCount = material->GetTextureCount(aiTextureType_SPECULAR);
		if (specularTextureCount > 0) {
			aiString texturePath;
			material->GetTexture(aiTextureType_SPECULAR, 0, &texturePath);
			if (strcmp(texturePath.C_Str(), "")) {
				materials[i].specularMapID = CreateTextureFromFile(directory + texturePath.C_Str());
			} else {
				LOG_WARNING("Diffuse Texure for: " << filename << " has an invalid filename");
			}
		} else {
			LOG_WARNING("Model: " << filename << " does not have a specular texture!");
		}

		if (material->GetTextureCount(aiTextureType_HEIGHT)) {
			aiString texturePath;
			material->GetTexture(aiTextureType_HEIGHT, 0, &texturePath);
			materials[i].normalMapID = CreateTextureFromFile(directory + texturePath.C_Str());
		} else {
			LOG_WARNING("Model: " << filename << " does not have a normal map");
		}

		//for (auto j = 0; j < matDiffuseCount; j++) {
		//	aiString texturePath;
		//	material->GetTexture(aiTextureType_DIFFUSE, j, &texturePath);
		//	model->diffuseTextures[diffuseIndex++] = (CreateTextureFromFile(directory + texturePath.C_Str()));
		//}

		//for (auto j = 0; j < matSpecularCount; j++) {
		//	aiString texturePath;
		//	material->GetTexture(aiTextureType_SPECULAR, j, &texturePath);
		//	model->specularTextures[specularIndex++] = (CreateTextureFromFile(directory + texturePath.C_Str()));
		//}
	}
	importer.FreeScene();
}

void MainLoop(F64 deltaTime) {
	if (PlatformGetKey(KEY_ESCAPE)) PlatformExit();
}

int main() {
	PlatformCreate("ModelViewer");
	PlatformRun(MainLoop);
	return 0;
}