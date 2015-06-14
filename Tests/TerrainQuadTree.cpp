#include "TerrainQuadTree.h"


float* TerrainQuadTree::heightmap;
int TerrainQuadTree::rootWidth;

TerrainQuadTree::TerrainQuadTree(Rectangle& bounds, float terrainHeight, float LODHeight, TerrainQuadTree* parent, int level) {
	this->bounds = bounds;
	this->terrainHeight = terrainHeight;
	this->LODDistance = LODHeight;
	this->parent = parent;
	this->level = level;

	auto thread = std::thread(GenerateTerrainTask, bounds.x, bounds.y, bounds.width, bounds.height, terrainHeight, this);
	thread.detach();
}

TerrainQuadTree::~TerrainQuadTree() {

	if (topLeft != nullptr){
		delete topLeft;
		delete topRight;
		delete bottomLeft;
		delete bottomRight;
	}
	else{
		delete mesh;
	}
}

bool TerrainQuadTree::Subdivde() {
	//The children were allready subdivied previously and nothing needs to be done...
	if (topLeft != nullptr || level == maxLevel) {
		return false;
	}
	//Make the quad trees
	topLeft = new TerrainQuadTree(Rectangle(bounds.x, bounds.y, bounds.width * 0.5f, bounds.height * 0.5f), terrainHeight * heightScalar, LODDistance * LODScalar, this, level + 1);
	topRight = new TerrainQuadTree(Rectangle(bounds.x + (bounds.width * 0.5f), bounds.y, bounds.width * 0.5f, bounds.height * 0.5f), terrainHeight * heightScalar, LODDistance * LODScalar, this, level + 1);
	bottomLeft = new TerrainQuadTree(Rectangle(bounds.x + (bounds.width * 0.5f), bounds.y + (bounds.height * 0.5f), bounds.width * 0.5f, bounds.height * 0.5f), terrainHeight * heightScalar, LODDistance * LODScalar, this, level + 1);
	bottomRight = new TerrainQuadTree(Rectangle(bounds.x, bounds.y + (bounds.height * 0.5f), bounds.width * 0.5f, bounds.height * 0.5f), terrainHeight * heightScalar, LODDistance * LODScalar, this, level + 1);

	delete mesh;
	mesh = nullptr;
	return true;
}

bool TerrainQuadTree::LODPosition(Vector3& position) {
	float dist1 = position.Distance(Vector3(bounds.x, 0, bounds.y));
	float dist2 = position.Distance(Vector3(bounds.x + bounds.width, 0, bounds.y));
	float dist3 = position.Distance(Vector3(bounds.x + bounds.width, 0, bounds.y + bounds.height));
	float dist4 = position.Distance(Vector3(bounds.x, 0, bounds.y + bounds.height));

	if (dist1 < LODDistance || dist2 < LODDistance || dist3 < LODDistance || dist4 < LODDistance) {
		if (topLeft == nullptr) {
			Subdivde();
			return true;
		}
		else {
			if (topLeft->LODPosition(position)) return true;
			if (topRight->LODPosition(position)) return true;
			if (bottomLeft->LODPosition(position)) return true;
			if (bottomRight->LODPosition(position)) return true;
		}
	}

	else if(topLeft != nullptr){
		Combine();
		return false;
	}

	return false;
}


bool TerrainQuadTree::Combine() {
	if (topLeft == nullptr) {
		if (parent == nullptr) {
			//This is the root tree of the structure... min LOD
			return false;
		}
		return parent->Combine();
	} 

	delete topLeft;
	delete topRight;
	delete bottomLeft;
	delete bottomRight;
	topLeft = nullptr;
	topRight = nullptr;
	bottomLeft = nullptr;
	bottomRight = nullptr;

	active = false;
	auto thread = std::thread(GenerateTerrainTask, bounds.x, bounds.y, bounds.width, bounds.height, terrainHeight, this);
	thread.detach();
}

void TerrainQuadTree::Draw(std::unique_ptr<IRenderer>& renderer, std::unique_ptr<GLSLProgram>& shader) {
	if (!active){
		if (mesh != nullptr){
			mesh->Setup();
			active = true;
		}
		else return;
	}
	
	if (topLeft != nullptr) {
		topLeft->Draw(renderer, shader);
		topRight->Draw(renderer, shader);
		bottomLeft->Draw(renderer, shader);
		bottomRight->Draw(renderer, shader);
	}
	else {
		renderer->Draw(mesh, shader);
	}
}


void TerrainQuadTree::GenerateTerrainTask(float terrainX, float terrainZ, float terrainWidth, float terrainLength, float terrainHeight, TerrainQuadTree* terrain) {
	const int meshResolution = 16;

	auto fbm = [](float x, float y, int octaves, float frequency, float persistance) {
		float total = 0.0f;
		float amplitude = 1.0f;
		float maxAmp = 0.0f;

		for (int i = 0; i < octaves; i++) {
			total += glm::simplex(glm::vec2(x * frequency, y * frequency)) * amplitude;
			frequency *= 2.0f;
			maxAmp += amplitude;
			amplitude *= persistance;
		}
		return total / maxAmp;
	};

	float gridWidth = terrainWidth / (float)meshResolution;
	float gridLength = terrainLength / (float)meshResolution;
	float heightmap[(meshResolution + 1) * (meshResolution + 1)];
	for (int i = 0; i <= meshResolution; i++) {
		for (int j = 0; j <= meshResolution; j++) {
			float x = terrainX + (gridWidth * i);
			float z = terrainZ + (gridLength * j);
			//heightmap[(j * meshResolution) + i] = terrainHeight * fbm(x, z, 6, 0.000025f, 0.5f);
			heightmap[(j * meshResolution) + i] = terrainHeight * glm::simplex(glm::vec2(x / (meshResolution * 3), z / (meshResolution * 3)));
		}
	}

	auto GetHeight = [&heightmap, &meshResolution](int x, int z) {
		return heightmap[(z * meshResolution) + x];
	};

	std::vector<Vertex3D> verticies;
	std::vector<GLuint> indices;
	for (int i = 0; i < meshResolution; i++) {
		for (int j = 0; j < meshResolution; j++) {
			float x = terrainX + (i * gridWidth);
			float z = terrainZ + (j * gridLength);
			verticies.push_back(Vertex3D(Vector3(x, GetHeight(i, j), z)));
			verticies.push_back(Vertex3D(Vector3(x + gridWidth, GetHeight(i + 1, j), z)));
			verticies.push_back(Vertex3D(Vector3(x + gridWidth, GetHeight(i + 1, j + 1), z + gridLength)));
			verticies.push_back(Vertex3D(Vector3(x, GetHeight(i, j + 1), z + gridLength)));

			int size = verticies.size();
			indices.push_back(size - 4);
			indices.push_back(size - 3);
			indices.push_back(size - 2);

			indices.push_back(size - 2);
			indices.push_back(size - 1);
			indices.push_back(size - 4);
		}
	}

	terrain->mutex.lock();
	terrain->mesh = new Mesh(verticies, indices);
	terrain->mutex.unlock();
}
