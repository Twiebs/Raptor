#pragma once

#include<Utils\CommonScene.h>

#include<glm\gtc\noise.hpp>

#include<thread>
#include<mutex>
#include<memory>

#include"TerrainQuadTree.h"

struct TerrainSector {
	unsigned int id;
	float x, z;
	float width, length;
	bool active = false;
	const static int meshResolution = 32;
	float heightmap[(meshResolution + 1) * (meshResolution + 1)];
	Mesh* mesh;
};

struct Terrain {
	std::mutex mutex;
	std::vector<TerrainSector*> activeSectors;
	std::vector<TerrainSector*> inactiveSectors;
};

class TerrainScene : public CommonScene{
public:
	TerrainScene();
	~TerrainScene();

	void OnLoad(Engine& engine) override;

	void Tick(float deltaTime) override;
	void Render(float deltaTime) override;

	bool OnKeyEvent(int keycode, bool isPressed, int mods) override;

	Terrain terrain;
	TerrainQuadTree* rootTree;
	std::vector<std::thread> threads;
private:
	bool debugCameraEnabled = false;


	Engine* engine;

	std::unique_ptr<FPSCamera> debugCamera;
	std::unique_ptr<GLSLProgram> terrainShader;
};

