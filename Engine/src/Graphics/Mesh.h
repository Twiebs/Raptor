#pragma once

#include<vector>

#include<GL\glew.h>

#include<Graphics\Texture2D.h>

#include<Math\Vector3.h>
#include<Math\Vector2.h>

#include<Core\Component.h>

struct Vertex {
	Vector3 position;
	Vector3 normal;
	Vector2 texCoords;

	Vertex(Vector3 position, Vector2 texCoords) {
		this->position = position;
		this->texCoords = texCoords;
	}
	
	Vertex(Vector3 position, Vector3 normal) {
		this->position = position;
		this->normal = normal;
	}

	Vertex(Vector3 position, Vector3 normal, Vector2 texCoords) {
		this->position = position;
		this->normal = normal;
		this->texCoords = texCoords;
	}
};

class Mesh : public Component{
public:
	GLuint vertexArrayID;
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture2D> textures;

	Mesh();
	Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indicies, std::vector<Texture2D>& textures);
	Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indicies);
	~Mesh();

	void CalculateNormals();

private:
	GLuint vertexBufferID;
	GLuint elementBufferID;

	void Setup();
};

