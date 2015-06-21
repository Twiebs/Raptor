#include "ECSTestScene.h"

#include<glm\gtc\matrix_transform.hpp>
#include<Math\Random.h>

#include<Core\TaskManager.hpp>
#include<Math\Noise.h>

ECSTestScene::ECSTestScene() {
	//glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


ECSTestScene::~ECSTestScene() {
}


void ECSTestScene::OnLoad(Engine& engine) {
	glm::mat4 projection = glm::ortho(0.0f, 1280.0f, 0.0f, 720.0f);
	auto renderSystem2D = engine.entityManager->CreateSystem<RenderSystem2D>(1000);
	renderSystem2D->SetProjectionMatrix(projection);

	Random rng;

	auto pixmapID = engine.assetManager->LoadPixmap("Assets/null.png");
	engine.taskManager->FinishAllTasksNow();
	auto pixmap = engine.assetManager->GetAsset<Pixmap>(pixmapID);
	
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pixmap->width, pixmap->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixmap->data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	auto fontID = engine.assetManager->LoadFont("Assets/fonts/arial.ttf", 15);

	for (int i = 0; i < 1999; i++) {
		EntityID entityID = engine.entityManager->CreateEntity();
		Entity* entity = engine.entityManager->GetEntity(entityID);
		SpriteComponent* sprite = entity->AddComponent<SpriteComponent>();
		sprite->textureID = textureID;
		sprite->x = rng.Range(20, 1200);
		sprite->y = rng.Range(20, 700);
		sprite->width = 256;
		sprite->height = 256;
		sprite->color = Color(1.0f, 1.0f, 1.0f, 1.0f);
	}

	for (int i = 0; i < 1999; i++) {
		EntityID entityID = engine.entityManager->CreateEntity();
		auto transform = engine.entityManager->CreateComponent<Transform2D>(entityID);
		transform->position.x = rng.Range(20, 1200);
		transform->position.y = rng.Range(20, 1200);

		TextComponent* textComponent = engine.entityManager->CreateComponent<TextComponent>(entityID);
		textComponent->x = rng.Range(20, 1200);
		textComponent->y = rng.Range(20, 700);
		std::string text;
		for (int i = 0; i < 6; i++)
			text.push_back(rng.Range(33, 126));
		textComponent->string = "This is most certianly a string";
		textComponent->color = Color(rng.Range(0.0f, 1.0f), rng.Range(0.0f, 1.0f), rng.Range(0.0f, 1.0f), rng.Range(0.0f, 1.0f));
		textComponent->fontID = fontID;
	}
}
void ECSTestScene::OnDestroy(Engine& engine) {

}
void ECSTestScene::Tick(float deltaTime)  {

}
void ECSTestScene::Render(float deltaTime) {

}