#pragma once

#include <ECS/EntityManager.hpp>
#include <Core/AssetManager.hpp>
#include <ECS/ISystem.hpp>
#include <ECS/ComponentBlock.hpp>

#include <Base/Transform2D.hpp>
#include <Base/SpriteComponent.hpp>
#include <Base/TextComponent.hpp>

#include <Graphics/Vertex2D.hpp>
#include <Graphics/GLTexture.hpp>
#include <Graphics/GLSLProgram.hpp>
#include <Assets/Font.hpp>

#include <Math/Matrix4.hpp>
#include <Math/Rectangle.hpp>
#include <Math/Vector2.hpp>

//TODO:
// - Batch sort mode
// - Multiple batch entries of render batches
// - Default "null" texture

//Renders entities in a 2D space
//Has an internal camera(NOT YET) that is used to project into screen space
//The camera can be controller by registering a camera controller with this system
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
	void SetProjectionMatrix(Matrix4& projection);

private:
	EntityManager* entityManager;
	uint32 batchCapacity;
	uint32 entryCount;

	uint8* memory;	//The memory used for the verticies and indicies
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

	Matrix4 projection;

	void Flush();
	void CheckTexture(GLuint textureID);

#ifdef BENCHMARK
	float64 textProcessTime;
	float64 spriteProcessTime;
#endif
};
