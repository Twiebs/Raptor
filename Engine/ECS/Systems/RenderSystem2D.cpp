#include "RenderSystem2D.hpp"

RenderSystem2D::RenderSystem2D(uint32 batchCapacity) :
	batchCapacity(batchCapacity)
{
	REGISTER_SYSTEM_TAG(RenderSystem2D);
	vertices = new Vertex2D[batchCapacity * 4];
	indices = new GLuint[batchCapacity * 6];

	for (int i = 0, vertIndex = 0; i < batchCapacity * 6; i += 6, vertIndex += 4) {
		indices[i + 0] = vertIndex + 0;
		indices[i + 1] = vertIndex + 3;
		indices[i + 2] = vertIndex + 2;

		indices[i + 3] = vertIndex + 2;
		indices[i + 4] = vertIndex + 1;
		indices[i + 5] = vertIndex + 0;
	}

	shader = new GLSLProgram("Resources/shaders/RenderSystem2D.vert", "Resources/shaders/RenderSystem2D.frag");
	isTextUniformLoc = shader->GetUniformLocation("isText");
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	glGenBuffers(1, &vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, (batchCapacity * 4) * sizeof(Vertex2D), NULL, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &elementBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (batchCapacity * 6) * sizeof(GLuint), &indices[0], GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (GLvoid*)offsetof(Vertex2D, position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (GLvoid*)offsetof(Vertex2D, uv));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (GLvoid*)offsetof(Vertex2D, color));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

RenderSystem2D::~RenderSystem2D() {
	glDeleteVertexArrays(1, &vertexArrayID);
	glDeleteBuffers(1, &vertexBufferID);
	glDeleteBuffers(1, &elementBufferID);
	//TODO: delete / unref the shader
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
		SpriteComponent* component = (SpriteComponent*)block->Get(0);
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
	if (block != nullptr){
		uint32 count = block->Count();
		TextComponent* component = (TextComponent*)block->Get(0);
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
	glBufferSubData(GL_ARRAY_BUFFER, 0, 4 * sizeof(Vertex2D) * entryCount, (GLvoid*)&vertices[0]);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glBindVertexArray(vertexArrayID);
	glDrawElements(GL_TRIANGLES, 6 * entryCount * 4, GL_UNSIGNED_INT, (GLvoid*)indices[0]);
	glBindVertexArray(0);
	entryCount = 0;
}


void RenderSystem2D::ProcessText(TextComponent& textComponent) {
	Font* font = AssetManager::Instance().GetAsset<Font>(textComponent.fontID);
	if (font == nullptr) return;

	CheckTexture(font->textureID);
	auto cursorPos = textComponent.x;
	for (uint32 i = 0; i < textComponent.text.size(); i++) {
		Glyph& glyph = font->glyphs[textComponent.text[i] - 32];
		auto x = cursorPos + glyph.berringX;
		auto y = textComponent.y - (glyph.height - glyph.bearingY);

		auto vertexIndex = entryCount * 4;
		vertices[vertexIndex++] = Vertex2D{ Vector2(x, y), glyph.uvs[0], textComponent.color};
		vertices[vertexIndex++] = Vertex2D{ Vector2(x + glyph.width, y), glyph.uvs[1], textComponent.color };
		vertices[vertexIndex++] = Vertex2D{ Vector2(x + glyph.width, y + glyph.height), glyph.uvs[2], textComponent.color };
		vertices[vertexIndex++] = Vertex2D{ Vector2(x, y + glyph.height), glyph.uvs[3], textComponent.color };

		cursorPos += glyph.advance;
		entryCount++;
		if ((entryCount + 1) == batchCapacity)
			Flush();
	}
}

void RenderSystem2D::ProcessSprite(SpriteComponent& sprite) {
	CheckTexture(sprite.textureID);
	auto vertexIndex = entryCount * 4;
	vertices[vertexIndex++] = Vertex2D{ Vector2(sprite.x, sprite.y), Vector2(0.0f, 0.0f), sprite.color };
	vertices[vertexIndex++] = Vertex2D{ Vector2(sprite.x + sprite.width, sprite.y), Vector2(1.0f, 0.0f), sprite.color };
	vertices[vertexIndex++] = Vertex2D{ Vector2(sprite.x + sprite.width, sprite.y + sprite.height), Vector2(1.0f, 1.0f), sprite.color };
	vertices[vertexIndex++] = Vertex2D{ Vector2(sprite.x, sprite.y + sprite.height), Vector2(0.0f, 1.0f), sprite.color };
	entryCount++;

	if ((entryCount + 1) == batchCapacity)
		Flush();
}

void RenderSystem2D::SetProjectionMatrix(glm::mat4& matrix) {
	shader->Use();
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

bool RenderSystem2D::Startup(EntityManager* manager) {
	textComponentID = manager->ComponentTypeOf<TextComponent>()->index;
	spriteComponentID = manager->ComponentTypeOf<SpriteComponent>()->index;
	return true;
}

bool RenderSystem2D::Shutdown(EntityManager* manager) {
	return true;
}