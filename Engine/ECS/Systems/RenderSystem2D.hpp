#pragma once

#include <Core\Preprocess.hpp>

#include <ECS\EntityManager.hpp>
#include <ECS\ISystem.hpp>
#include <ECS\Components\SpriteComponent.hpp>
#include <ECS\Components\TextComponent.hpp>
#include <ECS\ComponentBlock.hpp>

#include <Graphics\Vertex2D.hpp>
#include <Graphics\GL\GLTexture.h>
#include <Math\Rectangle.h>
#include <Math\Vector2.hpp>

class RenderSystem2D : public ISystem {
public:
	RenderSystem2D(uint32 batchSize = 2000);
	~RenderSystem2D();

	bool Startup (EntityManager* manager) final;
	bool Shutdown(EntityManager* manager) final;
	void Update(double deltaTime) final;

	void ProcessText(TextComponent& textComponent);
	void ProcessSprite(SpriteComponent& spriteComponent);
	void SetProjectionMatrix(glm::mat4& matrix);

private:
	friend class EntityManager;
	uint32 batchCapacity;
	uint32 entryCount;

	GLuint vertexArrayID;
	GLuint vertexBufferID;
	GLuint elementBufferID;
	GLuint currentTextureID;

	Vertex2D* vertices;
	GLuint* indices;

	uint32 spriteComponentID;
	uint32 textComponentID;

	GLSLProgram* shader;
	GLuint isTextUniformLoc;

	void Flush();
	void CheckTexture(GLuint textureID);

#ifdef BENCHMARK
	float64 textProcessTime;
	float64 spriteProcessTime;
#endif
};
