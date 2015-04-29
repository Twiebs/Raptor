#include "Mesh.h"

using namespace std;

Mesh::Mesh() {
}

Mesh::Mesh(vector<Vertex>& vertices, vector<GLuint>& indices, vector<Texture2D>& textures) {
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	this->Setup();
}

Mesh::Mesh(vector<Vertex>& vertices, vector<GLuint>& indices) {
	this->vertices = vertices;
	this->indices = indices;

	this->Setup();
}

Mesh::~Mesh() {
}

void Mesh::CalculateNormals() {
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

	for each (Vertex vertex in vertices) {
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
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	//EBO
	glGenBuffers(1, &elementBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

	//Vertex Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//Vertex Normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(1);

	//Vertex Texture Coord
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texCoords));
	
	//Disable buffers
	glBindVertexArray(0);
}
