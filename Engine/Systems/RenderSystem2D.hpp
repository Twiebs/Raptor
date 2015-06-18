#pragma once

#include <Core/EntityManager.hpp>
#include <Core/ISystem.hpp>
#include <Core/ComponentBlock.hpp>

#include <Components/Transform2D.hpp>
#include <Components/SpriteComponent.hpp>
#include <Components/TextComponent.hpp>

#include <Graphics/Camera.h>
#include <Graphics/Vertex2D.hpp>
#include <Graphics/GL\GLTexture.h>

#include <Math/Rectangle.h>
#include <Math/Vector2.hpp>

//TODO:
//Batch sort mode
//Multiple batch entries of render batches
//Default "null" texture


class RenderSystem2D : public ISystem {
friend class EntityManager;
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
	uint32 batchCapacity;
	uint32 entryCount;

	byte* memory;	//The memory used for the verticies and indicies
	Vertex2D* vertices;	//The ptr to the verticies in memory
	GLuint* indices;	//ptr offset into memory of the size of verticies

	GLuint vertexArrayID;
	GLuint vertexBufferID;
	GLuint elementBufferID;
	GLuint currentTextureID;

	uint32 transformComponentID;
	uint32 spriteComponentID;
	uint32 textComponentID;

	AssetID shaderID;
	GLuint isTextUniformLoc;

	glm::mat4 projection;

	void Flush();
	void CheckTexture(GLuint textureID);

#ifdef BENCHMARK
	float64 textProcessTime;
	float64 spriteProcessTime;
#endif
};
