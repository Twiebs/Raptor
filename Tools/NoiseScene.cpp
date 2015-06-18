#include "NoiseScene.h"
#include<Graphics\Color.h>
#include <Assets/Pixmap.hpp>
#include<Graphics\GL\GLTexture.h>



VerticalLayoutGroup::VerticalLayoutGroup(EntityManager* manager) {
	this->manager = manager;
	textComponentID = manager->ComponentTypeOf<TextComponent>()->index;
}

VerticalLayoutGroup::~VerticalLayoutGroup() {

}

bool VerticalLayoutGroup::InputEvent(double cursorX, double cursorY) {
	if (cursorX > x && cursorX < (x + width) && cursorY > y && cursorY < (y + height)) {
		return true;
	}
	return false;
}

void VerticalLayoutGroup::AddEntity(EntityID id) {
	entities.push_back(id);
	//For now assume each of these entites has a text component...

	auto transform = manager->GetComponent<Transform2D>(id);


	uint32 xpos = x;
	uint32 ypos = y;

	if (entities.size() == 0)
		return;

	for (uint32 i = (entities.size() - 1); i > 0; i--) {
		EntityID& id = entities[i];
		uint32 nextYInc = 30;

		auto sprite = manager->GetComponent<SpriteComponent>(id);
		if (sprite != nullptr) {
			sprite->x = xpos;
			sprite->y = ypos;
			sprite->width  = width;
			sprite->height = height;
		}
		  
		auto text = manager->GetComponent<TextComponent>(id);
		text->x = xpos;
		text->y = ypos;
		ypos -= nextYInc;
		width = 200;
		height = ypos;
	}
}


NoiseScene::NoiseScene() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glGenTextures(1, &noiseTextureID);
	glBindTexture(GL_TEXTURE_2D, noiseTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 128, 128, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
}


NoiseScene::~NoiseScene() {

}

void NoiseScene::OnLoad(Engine& engine){
	this->engine = &engine;
	rng.Seed(0);
	//GenerateTexture(6, 0.005f, 0.5f);

	engine.entityManager->RegisterComponent<Transform2D>(0);
	engine.entityManager->RegisterComponent<SpriteComponent>(0);
	engine.entityManager->RegisterComponent<TextComponent>(0);

	auto projection = glm::ortho(0.0f, 1280.0f, 0.0f, 720.0f);
	auto renderer = engine.entityManager->CreateSystem<RenderSystem2D>();
	renderer->SetProjectionMatrix(projection);

	auto fontID = engine.assetManager->LoadFont("Resources/fonts/arial.ttf", 18);
	auto btnTexture = engine.assetManager->LoadPixmap("Resources/null.png");
	engine.taskManager->FinishAllTasksNow();

	auto pixmap = engine.assetManager->GetAsset<Pixmap>(btnTexture);

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pixmap->width, pixmap->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixmap->data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	EntityManager* manager = engine.entityManager.get();

	group = new VerticalLayoutGroup(engine.entityManager.get());
	group->x = 10;
	group->y = 680;

	seedText = engine.entityManager->CreateEntity();
	engine.entityManager->CreateComponent<TextComponent>(seedText, "Seed: XXX", fontID);
	engine.entityManager->CreateComponent<Transform2D>(seedText);
	octavesText = engine.entityManager->CreateEntity();
	engine.entityManager->CreateComponent<TextComponent>(octavesText, "Octaves: XXX", fontID);
	engine.entityManager->CreateComponent<Transform2D>(octavesText);
	frequencyText = engine.entityManager->CreateEntity();
	engine.entityManager->CreateComponent<Transform2D>(frequencyText);
	engine.entityManager->CreateComponent<TextComponent>(frequencyText, "Frequency: X.X", fontID);

	auto generateButton = engine.entityManager->CreateEntity();
	auto btnSprite = manager->CreateComponent<SpriteComponent>(generateButton);
	btnSprite->x = 100;
	btnSprite->y = 200;
	btnSprite->width = 10;
	btnSprite->height = 10;
	btnSprite->textureID = textureID;
	auto clickText = manager->CreateComponent<TextComponent>(generateButton, "Click me!", fontID);
	clickText->x = 20;
	clickText->y = 40;

	group->AddEntity(seedText);
	group->AddEntity(octavesText);
	group->AddEntity(frequencyText);
	//group->AddEntity(generateButton);

	auto entity = engine.entityManager->CreateEntity();
	auto sprite = engine.entityManager->CreateComponent<SpriteComponent>(entity);
	sprite->width = 680;
	sprite->height = 680;
	sprite->x = (engine.app->GetWidth() * 0.5f) - (680 * 0.5f);
	sprite->y = (engine.app->GetHeight() * 0.5f) - (680 * 0.5f);
	sprite->textureID = noiseTextureID;
}

void NoiseScene::GenerateTexture(uint32 seed, int octaves, float frequency, float persistance) {
	Pixmap pixmap(128, 128, PixmapFormat::RGBA8);
	for (int x = 0; x < pixmap.width; x++){
		for (int y = 0; y < pixmap.height; y++){
			float n = (noise.RidgedNoise(x, y, octaves, frequency, persistance));
			uint32 i = ((4 * y) * pixmap.width) + (x * 4);
			pixmap.data[i + 0] = 255 * n;
			pixmap.data[i + 1] = 255 * n;
			pixmap.data[i + 2] = 255 * n;
			pixmap.data[i + 3] = 255 * 1.0f;
		}
	}

	engine->entityManager->GetComponent<TextComponent>(seedText)->string = "Seed: 0" + std::to_string(seed);
	engine->entityManager->GetComponent<TextComponent>(octavesText)->string = "Octaves: " + std::to_string(octaves);
	engine->entityManager->GetComponent<TextComponent>(frequencyText)->string = "Frequency: " + std::to_string(frequency);


	glBindTexture(GL_TEXTURE_2D, noiseTextureID);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 128, 128, GL_RGBA, GL_UNSIGNED_BYTE, pixmap.data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void NoiseScene::OnDestroy(Engine& engine){

}
void NoiseScene::Tick(float deltaTime) {

}

void NoiseScene::Render(float deltaTime) {
}

bool NoiseScene::OnKeyEvent(int keycode, bool isPressed, int mods) {
	if (!isPressed)
		return false;

	if (keycode == Key::Q) {
		std::cout << "Generating a new texture!" << "\n";
		GenerateTexture(0, rng.Range(1, 8), rng.Range(0.005f, 0.1f), 0.5f);
		return true;
	}

	return false;
}

bool NoiseScene::OnCursorPosEvent(double xpos, double ypos) {
	return false;
}

bool NoiseScene::OnMouseButtonEvent(int button, double xpos, double ypos, bool isPressed, int mods) {
	if (!isPressed) return false;	//Ingnore mouse up events

	if (button == MouseButton::BUTTON_LEFT) {
		group->InputEvent(xpos, ypos);
		return true;
	}
	return false;
}
