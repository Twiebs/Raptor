#pragma once

#include<vector>

#include<GL\glew.h>

#include<Math\Vertex3D.hpp>
#include<Math\Vector3.hpp>
#include<Math\Vector2.hpp>

#include<Graphics\GL\GLSLProgram.h>
#include<Graphics\GL\GLTexture.h>
#include<memory>


class Mesh {
public:
	GLuint vertexArrayID;

	//TODO mesh data should be private
	std::vector<Vertex3D> vertices;
	std::vector<GLuint> indices;
	std::vector<GLTexture*> textures;

	Mesh();
	Mesh(std::vector<Vertex3D>& vertices, std::vector<GLuint>& indicies, std::vector<GLTexture*>& textures);
	Mesh(std::vector<Vertex3D>& vertices, std::vector<GLuint>& indicies);
	~Mesh();

	void Setup();
	void Draw(std::unique_ptr<GLSLProgram>& shader);
	void CalculateNormals();
private:
	GLuint vertexBufferID;
	GLuint elementBufferID;
};

