#include "RenderSystem2D.hpp"

//Allocates memory for the verticies and indicies in the sprite batch and
//Creates vertex and element GLbuffers
RenderSystem2D::RenderSystem2D(uint32 batchCapacity) :
		batchCapacity(batchCapacity) {
	//Calculate the size in bytes of the vertices and indicies arrays
	uint32 verticesMemorySize = sizeof(Vertex2D) * batchCapacity * 4;
	uint32 indicesMemorySize = sizeof(GLuint) * batchCapacity * 6;
	//Allocate the memory the vertices and indices will use
	memory = new uint8[verticesMemorySize + indicesMemorySize];
	//Set the pointer of the vertices and incides to there offset into the memory block
	vertices = (Vertex2D*) memory;
	indices = (GLuint*) (memory + verticesMemorySize);

	//The indices will be static unlike the vertices so we set them now
	for (uint32 i = 0, vertIndex = 0; i < batchCapacity * 6; i += 6, vertIndex += 4) {
		//Triangle A
		indices[i + 0] = vertIndex + 0;
		indices[i + 1] = vertIndex + 3;
		indices[i + 2] = vertIndex + 2;

		//Triangle B
		indices[i + 3] = vertIndex + 2;
		indices[i + 4] = vertIndex + 1;
		indices[i + 5] = vertIndex + 0;
	}

	//Generate the GL buffers
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	//Generate the vertex buffer for the batch
	//Vertex data is reserved but not set... verticies are dynamicly added durring the processing of components
	glGenBuffers(1, &vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, verticesMemorySize, NULL, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (GLvoid*)offsetof(Vertex2D, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (GLvoid*)offsetof(Vertex2D, uv));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (GLvoid*)offsetof(Vertex2D, color));
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &elementBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesMemorySize, indices, GL_DYNAMIC_DRAW);

	glBindVertexArray(0);

	shader = new GLSLProgram(DEBUGLoadShaderFromFile("Assets/shaders/RenderSystem2D.vert", "Assets/shaders/RenderSystem2D.frag"));
	isTextUniformLoc = shader->GetUniformLocation("isText");
}

//TODO: How will the shader be handled?
RenderSystem2D::~RenderSystem2D() {
	delete[] memory;
	glDeleteVertexArrays(1, &vertexArrayID);
	glDeleteBuffers(1, &vertexBufferID);
	glDeleteBuffers(1, &elementBufferID);
}

//Called by the entityManager when this system is added
bool RenderSystem2D::Startup(ECSManager* manager) {
	//Get the id of the components that the system will process
	//transformComponentID = manager->ComponentTypeOf<Transform2D>()->index;
	textComponentID = manager->ComponentTypeOf<TextComponent>()->index;
	spriteComponentID = manager->ComponentTypeOf<SpriteComponent>()->index;
	REGISTER_SYSTEM_TAG (RenderSystem2D);

	// Get the id of the shader that we are going to use to renderer the entities in the system

	// shaderID = AssetManager::Instance().LoadShader("Assets/shaders/RenderSystem2D.vert", "Assets/shaders/RenderSystem2D.frag");
	this->manager = manager;
	return true;
}

bool RenderSystem2D::Shutdown(ECSManager* manager) {
	return true;
}

void RenderSystem2D::Update(double deltaTime) {
	entryCount = 0;
#ifdef BENCHMARK
	auto start = std::chrono::steady_clock::now();
#endif

	shader->Use();
	glUniform1i(isTextUniformLoc, 0);
	ComponentBlock* block = manager->GetComponentBlock(spriteComponentID);
	if (block != nullptr) {
		uint32 count = block->Count();
		SpriteComponent* component = (SpriteComponent*) block->Get(0);
		for (int i = 0; i < count; i++, component++) {
			ProcessSprite(*component);
		}
	}
	Flush();

#ifdef BENCHMARK
	auto spriteEnd = std::chrono::steady_clock::now();
	auto spriteDiff = spriteEnd - start;
	lastUpdateTime = std::chrono::duration<double, CLOCK_RESOLUTION>(spriteDiff).count();
#endif

#ifdef BENCHMARK
	auto textStart = std::chrono::steady_clock::now();
#endif

	glUniform1i(isTextUniformLoc, 1);
	block = manager->GetComponentBlock(textComponentID);
	if (block != nullptr) {
		uint32 count = block->Count();
		TextComponent* component = (TextComponent*) block->Get(0);
		for (int i = 0; i < count; i++, component++) {
			ProcessText(*component);
		}
	}
	Flush();

#ifdef BENCHMARK
	auto end = std::chrono::steady_clock::now();
	auto diff = end - start;
	lastUpdateTime = std::chrono::duration<double, CLOCK_RESOLUTION>(diff).count();
#endif

#ifdef BENCHMARK
	auto textDiff = end - textStart;
	lastUpdateTime = std::chrono::duration<double, CLOCK_RESOLUTION>(textDiff).count();
#endif

}

void RenderSystem2D::Flush() {
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 4 * sizeof(Vertex2D) * entryCount, (GLvoid*)vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(vertexArrayID);
	glDrawElements(GL_TRIANGLES, 6 * entryCount * 4, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	entryCount = 0;
}

void RenderSystem2D::ProcessText(TextComponent& textComponent) {
	Font* font = nullptr;
	//Font* font = AssetManager::Instance().GetAsset<Font>(textComponent.fontID);
	if (font == nullptr)
		return;

	//auto transform = manager->GetComponent<Transform2D>(textComponent.ownerID);
	//auto transformX = transform->position.x;
	//auto transformY = transform->position.y;

	CheckTexture(font->textureID);
	auto cursorPos = textComponent.x;
	for (uint32 i = 0; i < textComponent.string.size(); i++) {
		Glyph& glyph = font->glyphs[textComponent.string[i] - 32];
		auto x = cursorPos + glyph.bearingX;
		auto y = textComponent.y - (glyph.height - glyph.bearingY);

		auto vertexIndex = entryCount * 4;
		vertices[vertexIndex++] = Vertex2D { Vector2(x, y), glyph.uvs[0], textComponent.color };
		vertices[vertexIndex++] = Vertex2D { Vector2(x + glyph.width, y), glyph.uvs[1], textComponent.color };
		vertices[vertexIndex++] = Vertex2D { Vector2(x + glyph.width, y + glyph.height), glyph.uvs[2],
				textComponent.color };
		vertices[vertexIndex++] = Vertex2D { Vector2(x, y + glyph.height), glyph.uvs[3], textComponent.color };

		cursorPos += glyph.advance;
		entryCount++;
		if ((entryCount + 1) == batchCapacity)
			Flush();
	}
}

void RenderSystem2D::ProcessSprite(SpriteComponent& sprite) {
	CheckTexture(sprite.textureID);
	auto vertexIndex = entryCount * 4;
	vertices[vertexIndex++] = Vertex2D { Vector2(sprite.x, sprite.y), Vector2(0.0f, 0.0f), sprite.color };
	vertices[vertexIndex++] =
			Vertex2D { Vector2(sprite.x + sprite.width, sprite.y), Vector2(1.0f, 0.0f), sprite.color };
	vertices[vertexIndex++] = Vertex2D { Vector2(sprite.x + sprite.width, sprite.y + sprite.height), Vector2(1.0f,
			1.0f), sprite.color };
	vertices[vertexIndex++] =
			Vertex2D { Vector2(sprite.x, sprite.y + sprite.height), Vector2(0.0f, 1.0f), sprite.color };
	entryCount++;

	if ((entryCount + 1) == batchCapacity)
		Flush();
}

void RenderSystem2D::SetProjectionMatrix(Matrix4& matrix) {
	projection = matrix;
	shader->SetMatrix4("mvp", matrix);
}

void RenderSystem2D::CheckTexture(GLuint textureID) {
	if (textureID == currentTextureID)
		return;

	Flush();
	currentTextureID = textureID;
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, currentTextureID);
}
