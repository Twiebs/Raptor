#include <Core/logging.h>

#include <Graphics/Mesh.hpp>
#include <Graphics/Model.hpp>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/mesh.h>
#include <assimp/scene.h>

// TODO we need a better way to specifiy alignments!
// Perhaps all allocation functions can take a specific
// allocator and the alloator could do it...  Then we could also
// have an easy way to alocate memory without using a global malloc
// This could make it much easier to store assets contigously in memory

#include <Engine/Assets.hpp>

//static Model* AllocateModel (U32 meshCount) {
//	size_t materialHandleSize = sizeof(MaterialHandle) * meshCount;
//	size_t indexCountForMeshSize = sizeof(U32) * meshCount;
//	Model* model = (Model*)malloc(sizeof(Model) + materialHandleSize + indexCountForMeshSize);
//	model->meshMaterialHandles = (MaterialHandle*)(model + 1);
//	model->meshIndexCounts = (U32*)model->meshMaterialHandles + meshCount;
//	model->meshCount = meshCount;
//	return model;
//}

static Model CreateModel (U32 meshCount) {
	size_t materialHandleSize = sizeof(MaterialHandle) * meshCount;
	size_t indexCountForMeshSize = sizeof(U32) * meshCount;
	
	Model model;
	model.meshCount = meshCount;
	model.meshMaterialHandles = (MaterialHandle*)malloc(materialHandleSize + indexCountForMeshSize);
	model.meshIndexCounts = (U32*)(model.meshMaterialHandles + meshCount);
	return model;
}


static void ImportMeshData (MeshData* data, aiMesh* ai_mesh, U32 vertexOffset, U32 indexOffset) {
	assert(data->vertices != nullptr && data->vertexCount != 0);
	assert(data->indices != nullptr && data->indexCount != 0);
	assert(data->vertexCount >= (vertexOffset + ai_mesh->mNumVertices));
	assert(data->indexCount >= indexOffset + (ai_mesh->mNumFaces * 3));

	for (U32 i = 0; i < ai_mesh->mNumVertices; i++) {
		auto index = vertexOffset + i;
		data->vertices[index].position.x = ai_mesh->mVertices[i].x;
		data->vertices[index].position.y = ai_mesh->mVertices[i].y;
		data->vertices[index].position.z = ai_mesh->mVertices[i].z;
		data->vertices[index].normal.x = ai_mesh->mNormals[i].x;
		data->vertices[index].normal.y = ai_mesh->mNormals[i].y;
		data->vertices[index].normal.z = ai_mesh->mNormals[i].z;
		data->vertices[index].tangent.x = ai_mesh->mTangents[i].x;
		data->vertices[index].tangent.y = ai_mesh->mTangents[i].y;
		data->vertices[index].tangent.z = ai_mesh->mTangents[i].z;
		data->vertices[index].texCoord.x = ai_mesh->mTextureCoords[0][i].x;
		data->vertices[index].texCoord.y = ai_mesh->mTextureCoords[0][i].y;
	}

	for (U32 ai_index = 0, i = indexOffset; i < (ai_mesh->mNumFaces * 3) + indexOffset; ai_index++, i += 3) {
		data->indices[i + 0] = ai_mesh->mFaces[ai_index].mIndices[0];
		data->indices[i + 1] = ai_mesh->mFaces[ai_index].mIndices[1];
		data->indices[i + 2] = ai_mesh->mFaces[ai_index].mIndices[2];
	}
}


void ImportModelData (ModelData* data, const std::string& filename) {
	Assimp::Importer importer;
	auto scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenNormals);
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		LOG_ERROR("Assimp failed to load file (" << filename << ") " << importer.GetErrorString());
		return;
	}

	data->meshIndexCounts.reserve(scene->mNumMeshes);
	data->materialInfos.reserve(scene->mNumMaterials);
	data->meshCount = scene->mNumMeshes;

	U32 vertexCount = 0;
	U32 indexCount = 0;
	for (U32 i = 0; i < scene->mNumMeshes; i++) {
		auto aiMesh = scene->mMeshes[i];
		vertexCount += aiMesh->mNumVertices;
		indexCount += aiMesh->mNumFaces * 3;

		data->meshIndexCounts.push_back (aiMesh->mNumFaces * 3);
	}

	// data->meshData = AllocateMeshData(vertexCount, indexCount);
	AllocateMeshData(&data->meshData, vertexCount, indexCount);

	auto GetMaterialAssetInfo = [&filename](aiMaterial* material, U32 materialIndex) -> MaterialAssetInfo{
		auto lastSlashInFilepath = filename.find_last_of("/");
		auto lastDotInFilepath = filename.find_last_of('.');
		auto directory = filename.substr(0, lastSlashInFilepath + 1);
		auto name = filename.substr(lastSlashInFilepath + 1, lastDotInFilepath - (lastSlashInFilepath + 1));

		static auto getTextureFilename = [&directory](aiMaterial* material, aiTextureType type) -> std::string {
			auto textureCount = material->GetTextureCount(type);
			assert(textureCount <= 1 && "Two maps of the same kind are not currently suported!");
			if (textureCount == 0) return "";

			aiString textureFilename;
			material->GetTexture(type, 0, &textureFilename);
			return std::string(textureFilename.C_Str());
		};

		MaterialAssetInfo materialInfo;
		materialInfo.diffuseTextureFilename = getTextureFilename(material, aiTextureType_DIFFUSE);
		materialInfo.normalTextureFilename = getTextureFilename(material, aiTextureType_NORMALS);
		materialInfo.specularTextureFilename = getTextureFilename(material, aiTextureType_SPECULAR);
		materialInfo.name = name + ".material." + std::to_string(materialIndex);
		materialInfo.directory = directory;
		return materialInfo;
	};

	vertexCount = 0;
	indexCount = 0;

	std::vector<U32> importedMaterials;
	importedMaterials.reserve(scene->mNumMaterials);
	for (U32 i = 0; i < scene->mNumMeshes; i++) {
		auto aiMesh = scene->mMeshes[i];
		ImportMeshData(&data->meshData, aiMesh, vertexCount, indexCount);

		bool importMaterial = true;
		for (U32 n = 0; n < importedMaterials.size(); n++) {
			if (importedMaterials[n] == aiMesh->mMaterialIndex) importMaterial = false;
		}

		if (importMaterial) {
			data->materialInfos.emplace_back(GetMaterialAssetInfo(scene->mMaterials[aiMesh->mMaterialIndex], aiMesh->mMaterialIndex));
			importedMaterials.push_back(aiMesh->mMaterialIndex);
		}


		vertexCount += aiMesh->mNumVertices;
		indexCount += aiMesh->mNumFaces * 3;
	}
}

Model CreateModel (ModelData* data) {
	assert(data->meshCount > 0 && "Invalid Model Data provided!");
	auto model = CreateModel(data->meshCount);
	memcpy(model.meshIndexCounts, &data->meshIndexCounts[0], data->meshIndexCounts.size() * sizeof(U32));
	InitIndexedVertexBuffer(&model.indexedVertexBuffer, &data->meshData);
	return model;
}


//Model* ImportModel (const std::string& filename) {
//	Assimp::Importer importer;
//	auto scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenNormals);
//	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
//		LOG_ERROR("Assimp failed to load file (" << filename << ") " << importer.GetErrorString());
//		return false;
//	}
//
//	Model* model = AllocateModel(scene->mNumMeshes);
//
//	U32 vertexCount = 0;
//	U32 indexCount = 0;
//	for (U32 i = 0; i < scene->mNumMeshes; i++) {
//		auto aiMesh = scene->mMeshes[i];
//		vertexCount += aiMesh->mNumVertices;
//		indexCount += aiMesh->mNumFaces * 3;
//
//		model->meshIndexCounts[i] = aiMesh->mNumFaces * 3;
//	}
//
//	auto meshData = CreateMeshData(vertexCount, indexCount);
//
//
//	auto ImportMaterial = [&filename](aiMaterial* material, U32 materialIndex) -> MaterialHandle {
//		auto lastSlashInFilepath = filename.find_last_of("/");
//		auto lastDotInFilepath = filename.find_last_of('.');
//		auto directory = filename.substr(0, lastSlashInFilepath + 1);
//		auto name = filename.substr(lastSlashInFilepath + 1, lastDotInFilepath - (lastSlashInFilepath + 1));
//
//		static auto getTextureFilename = [&directory](aiMaterial* material, aiTextureType type) -> std::string {
//			auto textureCount = material->GetTextureCount(type);
//			assert(textureCount <= 1 && "Two maps of the same kind are not currently suported!");
//			if (textureCount == 0) return "";
//
//			aiString textureFilename;
//			material->GetTexture(type, 0, &textureFilename);
//			return std::string(textureFilename.C_Str());
//		};
//
//		MaterialAssetInfo materialInfo;
//		materialInfo.diffuseTextureFilename = getTextureFilename(material, aiTextureType_DIFFUSE);
//		materialInfo.normalTextureFilename = getTextureFilename(material, aiTextureType_NORMALS);
//		materialInfo.specularTextureFilename = getTextureFilename(material, aiTextureType_SPECULAR);
//		materialInfo.name = name + ".material." + std::to_string(materialIndex);
//		materialInfo.directory = directory;
//
//		auto materialHandle = LoadMaterial(materialInfo);
//		return materialHandle;
//	};
//
//	vertexCount = 0;
//	indexCount = 0;
//
//	std::vector<U32> importedMaterials;
//	for (U32 i = 0; i < scene->mNumMeshes; i++) {
//		auto aiMesh = scene->mMeshes[i];
//		ImportMeshData(meshData, aiMesh, vertexCount, indexCount);
//
//		bool importMaterial = true;
//		for (U32 n = 0; n < importedMaterials.size(); n++) {
//			if (importedMaterials[n] == aiMesh->mMaterialIndex) importMaterial = false;
//		}
//
//		if (importMaterial) {
//			auto materialHandle = ImportMaterial(scene->mMaterials[aiMesh->mMaterialIndex], aiMesh->mMaterialIndex);
//			model->meshMaterialHandles[i] = materialHandle;
//			importedMaterials.push_back(aiMesh->mMaterialIndex);
//		}
//
//
//		vertexCount += aiMesh->mNumVertices;
//		indexCount += aiMesh->mNumFaces * 3;
//	}
//
//
//	InitIndexedVertexBuffer(&model->indexedVertexBuffer, meshData);
//	free(meshData);
//	return model;
//}


//
//void ImportModel (const std::string& filename) {
//	Assimp::Importer importer;
//	auto scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenNormals);
//	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
//		LOG_ERROR("Assimp failed to load file (" << filename << ") " << importer.GetErrorString());
//		return;
//	}
//
//	meshes.resize(scene->mNumMeshes);
//	meshMaterialIndex.resize(scene->mNumMeshes);
//	meshVertexBuffers.resize(scene->mNumMeshes);
//	for (U32 i = 0; i < scene->mNumMeshes; i++) {
//		auto aiMesh = scene->mMeshes[i];
//		meshMaterialIndex[i] = aiMesh->mMaterialIndex;
//		ImportMeshData(&meshes[i], aiMesh);
//		InitVertexBufferForMeshData(&meshVertexBuffers[i], meshes[i]);
//	}
//
//	auto directory = filename.substr(0, filename.find_last_of("/") + 1);
//	materials.resize(scene->mNumMaterials);
//
//	for (U32 i = 0; i < scene->mNumMaterials; i++) {
//		auto material = scene->mMaterials[i];
//
//		auto diffuseTextureCount = material->GetTextureCount(aiTextureType_DIFFUSE);
//		if (diffuseTextureCount > 0) {
//			aiString texturePath;
//			material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath);
//			if (strcmp(texturePath.C_Str(), "")) {
//				materials[i].diffuseMapID = CreateTextureFromFile(directory + texturePath.C_Str());
//			} else {
//				LOG_WARNING("Diffuse Texure for: " << filename << " has an invalid filename");
//			}
//		} else {
//			LOG_WARNING("Model: " << filename << " does not have a diffuse texture!");
//		}
//
//		auto specularTextureCount = material->GetTextureCount(aiTextureType_SPECULAR);
//		if (specularTextureCount > 0) {
//			aiString texturePath;
//			material->GetTexture(aiTextureType_SPECULAR, 0, &texturePath);
//			if (strcmp(texturePath.C_Str(), "")) {
//				materials[i].specularMapID = CreateTextureFromFile(directory + texturePath.C_Str());
//			} else {
//				LOG_WARNING("Diffuse Texure for: " << filename << " has an invalid filename");
//			}
//		} else {
//			LOG_WARNING("Model: " << filename << " does not have a specular texture!");
//		}
//
//		if (material->GetTextureCount(aiTextureType_HEIGHT)) {
//			aiString texturePath;
//			material->GetTexture(aiTextureType_HEIGHT, 0, &texturePath);
//			materials[i].normalMapID = CreateTextureFromFile(directory + texturePath.C_Str());
//		} else {
//			LOG_WARNING("Model: " << filename << " does not have a normal map");
//		}
//
//		//for (auto j = 0; j < matDiffuseCount; j++) {
//		//	aiString texturePath;
//		//	material->GetTexture(aiTextureType_DIFFUSE, j, &texturePath);
//		//	model->diffuseTextures[diffuseIndex++] = (CreateTextureFromFile(directory + texturePath.C_Str()));
//		//}
//
//		//for (auto j = 0; j < matSpecularCount; j++) {
//		//	aiString texturePath;
//		//	material->GetTexture(aiTextureType_SPECULAR, j, &texturePath);
//		//	model->specularTextures[specularIndex++] = (CreateTextureFromFile(directory + texturePath.C_Str()));
//		//}
//	}
//	importer.FreeScene();
//}