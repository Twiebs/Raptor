#include <Core/logging.h>

#include <Graphics/Mesh.hpp>
#include <Graphics/Model.hpp>
#include <Graphics/VertexArray.hpp>

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

	for (auto i = 0; i < ai_mesh->mNumVertices; i++) {
		auto index = vertexOffset + i;
		data->vertices[index].position.x = ai_mesh->mVertices[i].x;
		data->vertices[index].position.y = ai_mesh->mVertices[i].y;
		data->vertices[index].position.z = ai_mesh->mVertices[i].z;
	}

	for (auto i = 0; i < ai_mesh->mNumVertices; i++) {
		auto index = vertexOffset + i;
		data->vertices[index].normal.x = ai_mesh->mNormals[i].x;
		data->vertices[index].normal.y = ai_mesh->mNormals[i].y;
		data->vertices[index].normal.z = ai_mesh->mNormals[i].z;
	}

	if (ai_mesh->mTangents != nullptr) {
		for (auto i = 0; i < ai_mesh->mNumVertices; i++) {
auto index = vertexOffset + i;
data->vertices[index].tangent.x = ai_mesh->mTangents[i].x;
data->vertices[index].tangent.y = ai_mesh->mTangents[i].y;
data->vertices[index].tangent.z = ai_mesh->mTangents[i].z;
		}
	}

	if (ai_mesh->mTextureCoords[0] != nullptr) {
		for (auto i = 0; i < ai_mesh->mNumVertices; i++) {
			auto index = vertexOffset + i;
			data->vertices[index].texCoord.x = ai_mesh->mTextureCoords[0][i].x;
			data->vertices[index].texCoord.y = ai_mesh->mTextureCoords[0][i].y;
		}
	}

	for (U32 ai_index = 0, i = indexOffset; i < (ai_mesh->mNumFaces * 3) + indexOffset; ai_index++, i += 3) {
		data->indices[i + 0] = ai_mesh->mFaces[ai_index].mIndices[0];
		data->indices[i + 1] = ai_mesh->mFaces[ai_index].mIndices[1];
		data->indices[i + 2] = ai_mesh->mFaces[ai_index].mIndices[2];
	}
}


void ImportModelData (ModelData* data, const std::string& filename) {
	Assimp::Importer importer;
	auto scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_CalcTangentSpace);
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		LOG_ERROR("Assimp failed to load file (" << filename << ") " << importer.GetErrorString());
		return;
	}

	data->meshIndexCounts.reserve(scene->mNumMeshes);
	// data->materialInfos.reserve(scene->mNumMaterials);
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

	auto CreateMaterialInfo = [&filename](aiMaterial* material, U32 materialIndex) -> MaterialInfo {
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

		MaterialInfo materialInfo;
		materialInfo.diffuseTextureFilename = getTextureFilename(material, aiTextureType_DIFFUSE);
		materialInfo.normalTextureFilename = getTextureFilename(material, aiTextureType_NORMALS);
		materialInfo.specularTextureFilename = getTextureFilename(material, aiTextureType_SPECULAR);
		materialInfo.name = name + ".material." + std::to_string(materialIndex);
		materialInfo.directory = directory;
		return materialInfo;
	};

	vertexCount = 0;
	indexCount = 0;

	//std::vector<U32> importedMaterials;
	//importedMaterials.reserve(scene->mNumMaterials);
	for (U32 i = 0; i < scene->mNumMeshes; i++) {
		auto aiMesh = scene->mMeshes[i];
		ImportMeshData(&data->meshData, aiMesh, vertexCount, indexCount);



		//bool importMaterial = true;
		//for (U32 n = 0; n < importedMaterials.size(); n++) {
		//	if (importedMaterials[n] == aiMesh->mMaterialIndex) importMaterial = false;
		//}

		//if (importMaterial) {
		//	data->materialInfos.emplace_back(CreateMaterialInfo(scene->mMaterials[aiMesh->mMaterialIndex], aiMesh->mMaterialIndex));
		//	importedMaterials.push_back(aiMesh->mMaterialIndex);
		//}


		vertexCount += aiMesh->mNumVertices;
		indexCount += aiMesh->mNumFaces * 3;
	}

	data->materialCount = scene->mNumMaterials;
	for (auto i = 0; i < data->materialCount; i++) {
	
	}
}

Model CreateModel (ModelData* data) {
	assert(data->meshCount > 0 && "Invalid Model Data provided!");
	auto model = CreateModel(data->meshCount);
	memcpy(model.meshIndexCounts, &data->meshIndexCounts[0], data->meshIndexCounts.size() * sizeof(U32));

	CreateIndexedVertexArray<Vertex3D>(&model.indexedVertexBuffer,
		data->meshData.vertexCount, data->meshData.indexCount,
		GL_STATIC_DRAW, data->meshData.vertices, data->meshData.indices);
	
	return model;
}


#include <cereal/archives/binary.hpp>
#include <cereal/types/vector.hpp>

template<typename TArchive>
void serialize(TArchive& archive, ModelData& data) {
	archive(data.meshCount);
	archive(data.meshIndexCounts);
	archive(data.meshData);
	archive(data.materialName);
}


void SaveModelData(ModelData* data, const std::string& filename) {
	
}

void LoadModelData(ModelData* data, const std::string& filename) {

}