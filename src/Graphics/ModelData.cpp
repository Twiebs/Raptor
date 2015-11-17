#include "ModelData.hpp"
#include "Texture.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/mesh.h>
#include <assimp/scene.h>

#include <Core/logging.h>
//
//VertexBufferGroup::~VertexBufferGroup() {
//	glDeleteVertexArrays(1, &vertexArrayID);
//	glDeleteBuffers(1, &vertexBufferID);
//	glDeleteBuffers(1, &elementBufferID);
//}
//
//void InitVertexBufferForMeshData (VertexBufferGroup* group, const MeshData& mesh) {
//	glGenVertexArrays(1, &group->vertexArrayID);
//	glBindVertexArray(group->vertexArrayID);
//
//	glGenBuffers(1, &group->vertexBufferID);
//	glBindBuffer(GL_ARRAY_BUFFER, group->vertexBufferID);
//	glBufferData(GL_ARRAY_BUFFER, mesh.vertexCount * sizeof(Vertex3D), mesh.vertices, GL_STATIC_DRAW);
//
//	glGenBuffers(1, &group->elementBufferID);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, group->elementBufferID);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indexCount * sizeof(U32), mesh.indices, GL_STATIC_DRAW);
//
//	glEnableVertexAttribArray(0);
//	glEnableVertexAttribArray(1);
//	glEnableVertexAttribArray(2);
//	glEnableVertexAttribArray(3);
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)offsetof(Vertex3D, position));
//	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)offsetof(Vertex3D, normal));
//	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)offsetof(Vertex3D, texCoord));
//	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)offsetof(Vertex3D, tangent));
//	glBindVertexArray(0);
//}
//
//static void ImportMeshData (MeshData* data, aiMesh* ai_mesh) {
//	assert(data->vertices == nullptr);
//	data->vertexCount = ai_mesh->mNumVertices;
//	data->indexCount = ai_mesh->mNumFaces * 3;
//	size_t verticesMemorySize = sizeof(Vertex3D) * data->vertexCount;
//	size_t indicesMemorySize = sizeof(U32) * data->indexCount;
//	data->memblock = (U8*)malloc(verticesMemorySize + indicesMemorySize);
//	data->vertices = (Vertex3D*)data->memblock;
//	data->indices = (U32*)(data->memblock + verticesMemorySize);
//
//	if (!ai_mesh->mTextureCoords[0]) {
//		LOG_WARNING("Mesh Does not contain textureCoordiantes!");
//	}
//
//	for (U32 i = 0; i < ai_mesh->mNumVertices; i++) {
//		data->vertices[i].position.x = ai_mesh->mVertices[i].x;
//		data->vertices[i].position.y = ai_mesh->mVertices[i].y;
//		data->vertices[i].position.z = ai_mesh->mVertices[i].z;
//		data->vertices[i].normal.x = ai_mesh->mNormals[i].x;
//		data->vertices[i].normal.y = ai_mesh->mNormals[i].y;
//		data->vertices[i].normal.z = ai_mesh->mNormals[i].z;
//		if (ai_mesh->mTextureCoords[0]) {
//			data->vertices[i].texCoord.x = ai_mesh->mTextureCoords[0][i].x;
//			data->vertices[i].texCoord.y = ai_mesh->mTextureCoords[0][i].y;
//		}
//	}
//
//	for (U32 index = 0, i = 0; i < ai_mesh->mNumFaces * 3; index++, i += 3) {
//		data->indices[i + 0] = ai_mesh->mFaces[index].mIndices[0];
//		data->indices[i + 1] = ai_mesh->mFaces[index].mIndices[1];
//		data->indices[i + 2] = ai_mesh->mFaces[index].mIndices[2];
//	}
//}
//
//void DebugModelData::ImportFromFile(const std::string& filename) {
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
//			}
//			else {
//				LOG_WARNING("Diffuse Texure for: " << filename << " has an invalid filename");
//			}
//		}
//		else {
//			LOG_WARNING("Model: " << filename << " does not have a diffuse texture!");
//		}
//
//		auto specularTextureCount = material->GetTextureCount(aiTextureType_SPECULAR);
//		if (specularTextureCount > 0) {
//			aiString texturePath;
//			material->GetTexture(aiTextureType_SPECULAR, 0, &texturePath);
//			if (strcmp(texturePath.C_Str(), "")) {
//				materials[i].specularMapID = CreateTextureFromFile(directory + texturePath.C_Str());
//			}
//			else {
//				LOG_WARNING("Diffuse Texure for: " << filename << " has an invalid filename");
//			}
//		}
//		else {
//			LOG_WARNING("Model: " << filename << " does not have a specular texture!");
//		}
//
//		if (material->GetTextureCount(aiTextureType_HEIGHT)) {
//			aiString texturePath;
//			material->GetTexture(aiTextureType_HEIGHT, 0, &texturePath);
//			materials[i].normalMapID = CreateTextureFromFile(directory + texturePath.C_Str());
//		}
//		else {
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
