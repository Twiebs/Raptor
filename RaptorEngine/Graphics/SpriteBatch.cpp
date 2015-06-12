#include "SpriteBatch.h"
#include<SOIL\SOIL.h>

namespace Raptor{

SpriteBatch::SpriteBatch(int maxSprites) {
	vertices.resize(4 * maxSprites);

	int indexCount = 6 * maxSprites;
	int vert = 0;
	indices.resize(indexCount);
	for (int i = 0; i < indexCount; i += 6, vert += 4){
		indices[i] = vert + 0;
		indices[i + 1] = vert + 1;
		indices[i + 2] = vert + 2;

		indices[i + 3] = vert + 2;
		indices[i + 4] = vert + 3;
		indices[i + 5] = vert + 0;
	}

	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	glGenBuffers(1, &vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexArrayID);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vertex2D), NULL, GL_DYNAMIC_DRAW);


	glGenBuffers(1, &elementBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (GLvoid*)offsetof(Vertex2D, position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (GLvoid*)offsetof(Vertex2D, uv));

	glBindVertexArray(0);

	CreateTestObject();

	//TODO shader in resource manager to support multipule batches
	shader = std::make_unique<GLSLProgram>("Resources/shaders/sprite.vert", "Resources/shaders/sprite.frag");
}



//Delete the GLBuffers
SpriteBatch::~SpriteBatch() {
	//Delete GLbuffers
	glDeleteVertexArrays(1, &vertexArrayID);
	glDeleteBuffers(1, &vertexBufferID);
	glDeleteBuffers(1, &elementBufferID);
	//TODO Allow shader to presist in a resource manager if being used by other batches
	shader.release();
}

void SpriteBatch::SetSortMode(BatchSortMode mode){
	this->sortMode = mode;
}

void SpriteBatch::Begin(){
	if (beginCalled){
		std::cout << "ERROR: SpriteBatch::End() was not called before this begin!" << std::endl;
		return;
	}

	beginCalled = true;
	entries.clear();
}

void SpriteBatch::End(){
	if (!beginCalled){
		std::cout << "ERROR: SpriteBatch::Begin() must be called before end!" << std::endl;
		return;
	}
	beginCalled = false;
	Flush();
}

void SpriteBatch::SetProjectionMatrix(glm::mat4& matrix){
	this->projection = matrix;
}

void SpriteBatch::Draw(float x, float y, float width, float height, GLTexture* texture){
	if (!beginCalled){
		std::cout << "ERROR: SpriteBatch::Begin() must be called before drawing!" << std::endl;
		//TODO ERROR:: Sprite batch begin not called...
	}

	BatchEntry entry;
	entry.bounds = Rectangle(x, y, width, height);
	entry.texture = texture;
	entries.push_back(entry);
}

void SpriteBatch::Flush(){
	shader->Use();
	shader->SetMatrix4("mvp", projection);

	for (int i = 0, vertexCount = 0; i < entries.size(); i++, vertexCount += 4) {
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		vertices[vertexCount + 0] = Vertex2D{ Vector2(entries[i].bounds.x, entries[i].bounds.y), Vector2(0.0f, 0.0f) };
		vertices[vertexCount + 1] = Vertex2D{ Vector2(entries[i].bounds.x + entries[i].bounds.width, entries[i].bounds.y), Vector2(1.0f, 0.0f) };
		vertices[vertexCount + 2] = Vertex2D{ Vector2(entries[i].bounds.x + entries[i].bounds.width, entries[i].bounds.y + entries[i].bounds.height), Vector2(1.0f, 1.0f) };
		vertices[vertexCount + 3] = Vertex2D{ Vector2(entries[i].bounds.x, entries[i].bounds.y + entries[i].bounds.height), Vector2(0.0f, 1.0f) };
		glBufferSubData(GL_ARRAY_BUFFER, 0, 4 * sizeof(Vertex2D), &vertices[0 + vertexCount]);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, entries[i].texture->id);
		glBindVertexArray(vertexArrayID);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
}

void SpriteBatch::CreateTestObject(){
	Vertex2D verts[] {
		Vertex2D{ Vector2(25, 25), Vector2(0.0f, 0.0f) },
		Vertex2D{ Vector2(500, 25), Vector2(1.0f, 0.0f) },
		Vertex2D{ Vector2(500, 500), Vector2(1.0f, 1.0f) },

		Vertex2D{ Vector2(25, 25), Vector2(0.0f, 0.0f) },
		Vertex2D{ Vector2(500, 500), Vector2(1.0f, 1.0f) },
		Vertex2D{ Vector2(25, 500), Vector2(0.0f, 1.0f) },
	};
	
	glGenVertexArrays(1, &testVAO);
	glBindVertexArray(testVAO);

	glGenBuffers(1, &testVBO);
	glBindBuffer(GL_ARRAY_BUFFER, testVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (GLvoid*)offsetof(Vertex2D, position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (GLvoid*)offsetof(Vertex2D, uv));

	glBindVertexArray(0);


	//Texture

}

}//Namespace