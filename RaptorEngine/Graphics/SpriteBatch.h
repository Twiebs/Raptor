#pragma once

#include<iostream>
#include<memory>
#include<Graphics\GL\GLSLProgram.h>
#include<Graphics\GL\GLTexture.h>
#include<Math\Rectangle.h>

//TODO Abstract GL into GLSpriteBatch

namespace Raptor{

//Enum for the BatchSortMode
enum class BatchSortMode{
	NONE,
	DEPTH,
	TEXTURE,
	DEPTH_AND_TEXTURE
};

struct Vertex2D{
	Vector2 position;
	Vector2 uv;
};

struct BatchEntry {
	Rectangle bounds;
	GLTexture* texture;
};

struct RenderBatch {
	GLuint offset;
	GLuint vertexCount;
	GLTexture* texture;
};

class SpriteBatch {
public:
	SpriteBatch(int maxSprites = 1024);
	~SpriteBatch();

	void Begin();
	void End();

	void Draw(float x, float y, float width, float height, GLTexture* texture);
	void Draw(Rectangle bounds, GLTexture* texture);

	void SetSortMode(BatchSortMode mode);
	void SetProjectionMatrix(glm::mat4& matrix);

private:
	GLuint vertexArrayID;
	GLuint vertexBufferID;
	GLuint elementBufferID;
	BatchSortMode sortMode;

	std::vector<Vertex2D> vertices;
	std::vector<GLuint> indices;

	std::vector<BatchEntry> entries;

	bool beginCalled = false;

	glm::mat4 projection;
	std::unique_ptr<GLSLProgram> shader;

	GLuint nullTextureID;

	void Sort();
	void Flush();
	void CreateTestObject();

	GLuint testVAO;
	GLuint testVBO;
	GLuint testTexture;
};

}