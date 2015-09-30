#include "Assets.hpp"

namespace Raptor {

void CreateVertexBuffersForMesh(const MeshData& mesh, VertexBufferGroup& group) {
	glGenVertexArrays(1, &group.vertexArrayID);
	glBindVertexArray(group.vertexArrayID);

	glGenBuffers(1, &group.vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, group.vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, mesh.vertexCount * sizeof(Vertex3D), mesh.vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &group.elementBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, group.elementBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indexCount * sizeof(U32), mesh.indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)offsetof(Vertex3D, position));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)offsetof(Vertex3D, normal));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)offsetof(Vertex3D, texCoord));
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)offsetof(Vertex3D, tangent));
	glBindVertexArray(0);
}

//void LoadModel(const std::string& filename) {
//	global_assetTable.models.emplace_back(DebugModelData());
//	auto& model = global_assetTable.models[global_assetTable.models.size() - 1];
//	model.LoadFromFile(filename);
//}

void ImportModel(DebugModelData* model, const std::string& filename) {

}

void ImportModel(const std::string& filename) {
	
}



}