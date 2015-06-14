#include "ECSTestScene.h"

#include<Core\ResourceManager.h>
#include<glm\gtc\matrix_transform.hpp>
#include<Math\Random.h>

#include<Core\TaskManager.hpp>
#include<Core\Preprocess.hpp>
#include<Math\Noise.h>

ECSTestScene::ECSTestScene() {
	//glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


ECSTestScene::~ECSTestScene() {
}


void ECSTestScene::OnLoad(Engine& engine) {
	entityManager = new EntityManager();
	glm::mat4 projection = glm::ortho(0.0f, 1280.0f, 0.0f, 720.0f);
	auto renderSystem2D = entityManager->CreateSystem<RenderSystem2D>(1000);
	renderSystem2D->SetProjectionMatrix(projection);

	Random rng;

	std::shared_ptr<ImageData> data = AssetManager::Instance().GetImage("Resources/test.png");
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, data->width, data->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data->pixels);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);


	AssetID fontID = AssetManager::Instance().LoadFont("Resources/fonts/arial.ttf", 15);


	for (int i = 0; i < 1999; i++) {
		EntityID entityID = entityManager->CreateEntity();
		Entity& entity = entityManager->GetEntity(entityID);
		SpriteComponent* sprite = entity.AddComponent<SpriteComponent>();
		sprite->textureID = textureID;
		sprite->x = rng.Range(20, 1200);
		sprite->y = rng.Range(20, 700);
		sprite->width = 256;
		sprite->height = 256;
		sprite->color = Color(1.0f, 1.0f, 1.0f, 1.0f);
	}


	for (int i = 0; i < 1999; i++) {
		EntityID entityID = entityManager->CreateEntity();
		Entity& entity = entityManager->GetEntity(entityID);
		TextComponent* textComponent = entity.AddComponent<TextComponent>();
		textComponent->x = rng.Range(20, 1200);
		textComponent->y = rng.Range(20, 700);
		std::string text;
		for (int i = 0; i < 6; i++) {
			text.push_back(rng.Range(33, 126));
		}
		textComponent->text = "This is most certianly a string";
		textComponent->color = Color(rng.Range(0.0f, 1.0f), rng.Range(0.0f, 1.0f), rng.Range(0.0f, 1.0f), rng.Range(0.0f, 1.0f));
		textComponent->fontID = fontID;
	}
}
void ECSTestScene::OnDestroy(Engine& engine) {

}
void ECSTestScene::Tick(float deltaTime)  {
	glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	TaskManager::Instance().Update();
	entityManager->Update(deltaTime);
}
void ECSTestScene::Render(float deltaTime) {

}