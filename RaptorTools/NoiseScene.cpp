#include "NoiseScene.h"
#include<Graphics\Color.h>
#include<Graphics\Image.h>
#include<Graphics\GL\GLTexture.h>
#include<Core\ResourceManager.h>


NoiseSceneCanvas::NoiseSceneCanvas(float viewportWidth, float viewportHeight) : Canvas(viewportWidth, viewportHeight){

}
NoiseSceneCanvas::~NoiseSceneCanvas(){

}

void NoiseSceneCanvas::Render(float deltaTime){
	Canvas::Render(deltaTime);
}

NoiseScene::NoiseScene() {

}


NoiseScene::~NoiseScene() {

}

void NoiseScene::OnLoad(Engine& engine){
	engine.GetApp()->AddListener(this);

	batch = std::make_unique<SpriteBatch>();
	batch->SetProjectionMatrix(glm::ortho(0.0f, 1280.0f, 0.0f, 720.0f));

	canvas = std::make_unique<NoiseSceneCanvas>(1280.0f, 720.0f);

	GenerateTexture(6, 0.005f, 0.5f);
	texture = ResourceManager::GetGLTexture("Resources/test.png");

	rng.Seed(0);
}


void NoiseScene::GenerateTexture(int octaves, float frequency, float persistance){
	ImageData data;
	data.width = 128;
	data.height = 128;
	data.pixels = new RGBA8[data.width * data.height];
	for (int x = 0; x < data.width; x++){
		for (int y = 0; y < data.height; y++){
			float n = (noise.FBM(x, y, octaves, frequency, persistance));
			RGBA8 pixel;
			pixel.r = 255 * n;
			pixel.g = 255 * n;
			pixel.b = 255 * n;
			pixel.a = 255;
			data.pixels[(y * data.width) + x] = pixel;
		}
	}

	if (noiseTexture != nullptr){
		delete noiseTexture;
		noiseTexture = nullptr;
	}

	noiseTexture = new GLTexture(data);
}
void NoiseScene::OnDestroy(Engine& engine){

}
void NoiseScene::Tick(float deltaTime){

}
void NoiseScene::Render(float deltaTime){
	glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	batch->Begin();
	batch->Draw((1280 * 0.5f) - (256.0f * 0.5f), (720.0f * 0.5f) - (256.0f * 0.5f), 256.0f, 256.0f, noiseTexture);
	//batch->Draw(700.0f, 25.0f, 10.0f, 600.0f, texture);
	//batch->Draw(25.0f, 600.0f, 250.0f, 10.0f, *texture);
	batch->End();

	//canvas->Render(deltaTime);
}

bool NoiseScene::OnKeyEvent(int keycode, bool isPressed, int mods){
	if (!isPressed)
		return false;

	if (keycode == Key::Q){
		GenerateTexture(rng.Range(1, 8), rng.Range(0.005f , 0.5f), 0.5f);
		return true;
	}

	return false;
}
