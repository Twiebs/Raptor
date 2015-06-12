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

static void NoiseTest() {
	GLMSimplexNoise noise(0);
	for (int x = 0; x < 50; x++) {
		for (int y = 0; y < 50; y++) {
			auto value = noise.Eval(x, y);
		}
	}
}

static void TaskTest() {
	BENCHMARK_START(NoiseThreadTest);
	for (auto i = 0; i < 2; i++) {
		BENCHMARK_START(TaskScheduled);
		SCHEDULE_TASK(NoiseTest);
		BENCHMARK_END(TaskScheduled);
		BENCHMARK_LOG(TaskScheduled);
	}
	BENCHMARK_END(NoiseThreadTest);
	BENCHMARK_LOG(NoiseThreadTest);
}

void ECSTestScene::OnLoad(Engine& engine) {



	entityManager = new EntityManager();
	glm::mat4 projection = glm::ortho(0.0f, 1280.0f, 0.0f, 720.0f);
	auto renderSystem2D = entityManager->AddSystem<RenderSystem2D>(1000);
	renderSystem2D->SetProjectionMatrix(projection);

	Random rng;
	GLTexture* texture = ResourceManager::Instance().GetGLTexture("Resources/test.png");

	Font* font = ResourceManager::Instance().LoadFont("Resources/fonts/arial.ttf", 15);
	for (int i = 0; i < 1999; i++) {
		Entity* entity = entityManager->CreateEntity();
		SpriteComponent* sprite = entity->AddComponent<SpriteComponent>();
		sprite->textureID = texture->id;
		sprite->x = rng.Range(20, 1200);
		sprite->y = rng.Range(20, 700);
		sprite->width = 256;
		sprite->height = 256;
		sprite->color = Color(1.0f, 1.0f, 1.0f, 1.0f);
	}


	for (int i = 0; i < 1999; i++) {
		Entity* entity = entityManager->CreateEntity();
		TextComponent* textComponent = entity->AddComponent<TextComponent>();
		textComponent->x = rng.Range(20, 1200);
		textComponent->y = rng.Range(20, 700);
		std::string text;
		for (int i = 0; i < 6; i++) {
			text.push_back(rng.Range(33, 126));
		}
		textComponent->text = "This is most certianly a string";
		textComponent->color = Color(rng.Range(0.0f, 1.0f), rng.Range(0.0f, 1.0f), rng.Range(0.0f, 1.0f), rng.Range(0.0f, 1.0f));
		textComponent->font = font;
	}
}
void ECSTestScene::OnDestroy(Engine& engine) {

}
void ECSTestScene::Tick(float deltaTime)  {
	glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	entityManager->Update(deltaTime);

	static double time = 0;
	time += deltaTime;
	if (time > 7) {
		TaskTest();
		time = 0;
	}
}
void ECSTestScene::Render(float deltaTime) {

}