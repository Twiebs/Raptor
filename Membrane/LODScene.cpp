#include "LODScene.h"
#include<glm\gtc\noise.hpp>


LODScene::LODScene() {
}


LODScene::~LODScene() {
}


void LODScene::BuildSector(TerrainSector* sector, Terrain& terrain) {
	//auto fbm = [](float x, float y, int octaves, float frequency, float persistance) {
	//	float total = 0.0f;
	//	float amplitude = 1.0f;
	//	float maxAmp = 0.0f;

	//	for (int i = 0; i < octaves; i++) {
	//		total += glm::simplex(glm::vec2(x * frequency, y * frequency)) * amplitude;
	//		frequency *= 2.0f;
	//		maxAmp += amplitude;
	//		amplitude *= persistance;
	//	}
	//	return total / maxAmp;
	//};

	float gridWidth = sector->width / TerrainSector::meshResolution;
	float gridLength = sector->length / TerrainSector::meshResolution;

	sector->heightmap[TerrainSector::meshResolution * TerrainSector::meshResolution];
	for (int i = 0; i <= TerrainSector::meshResolution; i++) {
		for (int j = 0; j <= TerrainSector::meshResolution; j++) {
			//heightmap[(z * meshResolution) + x] = maxHeight * fbm(bounds.x + (x * gridWidth), bounds.y + (z * gridLength), 6, 0.0025f, 0.5f);
			sector->heightmap[(j * TerrainSector::meshResolution) + i] = 1.0f;
		}
	}


	auto GetHeight = [&sector](int x, int z) {
		return sector->heightmap[(z * TerrainSector::meshResolution) + x];
	};

	std::vector<Vertex> verticies;
	std::vector<GLuint> indices;

	for (int i = 0; i < TerrainSector::meshResolution; i++) {
		for (int j = 0; j < TerrainSector::meshResolution; j++) {
			float x = sector->x + (i * gridWidth);
			float z = sector->z + (j * gridLength);
			verticies.push_back(Vertex(Vector3(x, GetHeight(x, z), z)));
			verticies.push_back(Vertex(Vector3(x + gridWidth, GetHeight(x + gridWidth, z), z)));
			verticies.push_back(Vertex(Vector3(x + gridWidth, GetHeight(x + gridWidth, z + gridLength), z + gridLength)));
			verticies.push_back(Vertex(Vector3(x, GetHeight(x, z + gridLength), z + gridLength)));

			int size = verticies.size();
			indices.push_back(size - 4);
			indices.push_back(size - 3);
			indices.push_back(size - 2);

			indices.push_back(size - 2);
			indices.push_back(size - 1);
			indices.push_back(size - 4);
		}
	}

	//auto mesh = std::make_unique<Mesh>(verticies, indices);
	Mesh* mesh = new Mesh(verticies, indices);
	sector->mesh = mesh;
}



void LODScene::OnLoad(Engine& engine) {
	CommonScene::OnLoad(engine);
	camera->position.Set(0.0f, 12000.0f, 0.0f);
	engine.GetApp()->AddListener(this);

	terrainShader = std::make_unique<GLSLProgram>("Resources/shaders/Terrain.vert", "Resources/shaders/Terrain.frag");

	width = 512;
	length = 512;
	heightmap = new float[(width + 1)* (length + 1)];

	LODQuadTree::rootWidth = length;
	LODQuadTree::heightmap = heightmap;
	rootTree = std::make_unique<LODQuadTree>(Rectangle(0, 0, width, length), 500.0f, nullptr);
	selectedTree = rootTree.get();
}

float LODScene::GetHeight(float x, float z) {
	return heightmap[((int)z * width) + (int)x];
};



void LODScene::Tick(float deltaTime) {
	CommonScene::Tick(deltaTime);
	if (loadQueue.size() > 0) {
		Rectangle bounds = loadQueue.front();
		loadQueue.pop();

		TerrainSector* sector = new TerrainSector;
		sector->x = bounds.x;
		sector->z = bounds.y;
		sector->width = bounds.width;
		sector->length = bounds.height;

		auto t = std::thread(&BuildSector, sector, std::ref(terrain));
		threads.push_back(std::move(t));
	}
}

void LODScene::Render(float deltaTime) {
	rootTree->LODPosition(camera->position);

	CommonScene::Render(deltaTime);

	glm::mat4 mvp = camera->GetCombinedMatrix();
	terrainShader->Use();
	glUniformMatrix4fv(terrainShader->GetUniformLocation("mvp"), 1, GL_FALSE, &mvp[0][0]);
	terrain.mutex.lock();
	for (TerrainSector* sector: terrain.sectors) {
		//renderer->Draw(sector->mesh, terrainShader);
	}
	terrain.mutex.unlock();

	//rootTree->Draw(renderer, terrainShader);
}

bool LODScene::OnKeyEvent(Input::Keycode keycode, bool isPressed, int mod) {
	if (!isPressed) {
		return false;
	}

	switch (keycode) {
	case Input::KEY_F4:
		loadQueue.push(Rectangle(0, 0, 64, 64));
		return true;
	case Input::KEY_F3:
		renderer->ToggleWireframeEnabled();
		return true;
	case Input::KEY_F6:
		selectedTree->Subdivde();
		selectedTree = selectedTree->topLeft.get();
		return true;
	case Input::KEY_F7:
		if (selectedTree->Combine()) {
			selectedTree = selectedTree->parent;
		}
	}
	return false;
}
