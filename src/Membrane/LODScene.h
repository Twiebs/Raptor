#pragma once
#include <Utils\CommonScene.h>
#include <Graphics\Model.h>
#include<queue>

#include<mutex>
#include<thread>
#include<future>
#include<atomic>

#include "LODQuadtree.h"

struct TerrainSector {
	uint64 id;
	float x, z;
	float width, length;
	bool active = false;;
	const static int meshResolution = 32;;
	float heightmap[meshResolution * meshResolution];
	Mesh* mesh;
};

struct Terrain {
	std::mutex mutex;
	std::vector<TerrainSector*> sectors;
	const static float maxHeight;
};

class LODScene : public CommonScene, public IInputListener{
public:
	LODScene();
	~LODScene();

	void OnLoad(Engine& engine) override;
	void Tick(float deltaTime) override;
	void Render(float deltaTime) override;

	bool OnKeyEvent(Input::Keycode, bool isPressed, int mod) override;


private:
	GLuint vertexArrayID;
	GLuint vertexBufferID;
	GLuint elementBufferID;

	std::vector<GLuint> indices;
	std::vector<Vertex> verticies;
	std::queue<Rectangle> loadQueue;

	std::unique_ptr<GLSLProgram> terrainShader;
	std::vector<std::thread> threads;
	Terrain terrain;


	std::unique_ptr<LODQuadTree> rootTree;
	LODQuadTree* selectedTree;

	//Terrain Stuff
	int width, length;
	float* heightmap;

	void BuildMesh(float x, float y, float width, float height);
	float GetHeight(float x, float z);

	static void BuildSector(TerrainSector* sector, Terrain& terrain);
};

