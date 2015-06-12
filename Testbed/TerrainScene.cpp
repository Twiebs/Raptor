#include "TerrainScene.h"


TerrainScene::TerrainScene() {
}


TerrainScene::~TerrainScene() {
}

void TerrainScene::OnLoad(Engine& engine) {
	CommonScene::OnLoad(engine);
	this->engine = &engine;
	debugCamera = std::make_unique<FPSCamera>(engine.GetApp()->GetWidth(), engine.GetApp()->GetHeight());


	terrainShader = std::make_unique<GLSLProgram>("Resources/shaders/Terrain.vert", "Resources/shaders/Terrain.frag");
	camera->position.Set(0.0f, 1000.0f, 0.0f);
	rootTree = new TerrainQuadTree(Rectangle(0.0f, 0.0f, 4092.0f, 4092.0f), 256.0f, 4092.0f, nullptr, 0);
}

void TerrainScene::Tick(float deltaTime) {
	CommonScene::Tick(deltaTime);
	debugCamera->Update(deltaTime);
	rootTree->LODPosition(camera->position);
}

void TerrainScene::Render(float deltaTime) {
	CommonScene::Render(deltaTime);

	glm::mat4 mvp = camera->GetCombinedMatrix();
	terrainShader->Use();
	glUniformMatrix4fv(terrainShader->GetUniformLocation("mvp"), 1, GL_FALSE, &mvp[0][0]);
	rootTree->Draw(renderer, terrainShader);
}

bool TerrainScene::OnKeyEvent(int keycode, bool isPressed, int mods){
	CommonScene::OnKeyEvent(keycode, isPressed, mods);

	if (!isPressed)
		return false;

	switch (keycode){
	case Key::P:
		if (debugCameraEnabled){
			debugCameraEnabled = false;
			engine->GetApp()->AddListener(camera.get());
			engine->GetApp()->RemoveListener(debugCamera.get());
			renderer->SetCamera(camera.get());
		}
		else {
			debugCameraEnabled = true;
			engine->GetApp()->RemoveListener(camera.get());
			engine->GetApp()->AddListener(debugCamera.get());
			renderer->SetCamera(debugCamera.get());
		}
		return true;
	}
	return false;
}
