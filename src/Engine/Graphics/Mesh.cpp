#include "Mesh.hpp"

#include<sstream>

Mesh::Mesh() {
}

Mesh::Mesh(std::vector<Vertex3D>& vertices, std::vector<GLuint>& indices, std::vector<GLTexture*>& textures) {
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;
}

Mesh::Mesh(std::vector<Vertex3D>& vertices, std::vector<GLuint>& indices) {
	this->vertices = vertices;
	this->indices = indices;
	CalculateNormals();
}

Mesh::~Mesh() {
}

void Mesh::CalculateNormals() {
	//Ensure all vertex normals are zero before the calculation
	for (int i = 0; i < vertices.size(); i++) {
		vertices[i].normal = Vector3(0.0f, 0.0f, 0.0f);
	}
	for (unsigned int i = 0; i < indices.size(); i += 3) {
		unsigned int index0 = indices[i];
		unsigned int index1 = indices[i + 1];
		unsigned int index2 = indices[i + 2];

		Vector3 v1 = vertices[index1].position - vertices[index0].position;
		Vector3 v2 = vertices[index2].position - vertices[index0].position;

		Vector3 normal = v1.Cross(v2);
		normal.Normalize();

		vertices[index0].normal += normal;
		vertices[index1].normal += normal;
		vertices[index2].normal += normal;
	}

	for(Vertex3D& vertex : vertices) {
		vertex.normal.Normalize();
	}
}
void Mesh::Setup() {
	//Vertex Array
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	//VBO
	glGenBuffers(1, &vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex3D), &vertices[0], GL_STATIC_DRAW);

	//EBO
	glGenBuffers(1, &elementBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

	//Vertex Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//Vertex Normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)offsetof(Vertex3D, normal));
	glEnableVertexAttribArray(1);

	//Vertex Texture Coord
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)offsetof(Vertex3D, uv));
	
	//Disable buffers
	glBindVertexArray(0);
}
