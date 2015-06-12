#include "LODQuadTree.h"

LODQuadTree::LODQuadTree(Rectangle& bounds, float LODHeight, LODQuadTree* parent) {
	this->bounds = bounds;
	this->LODHeight = LODHeight;
	this->parent = parent;
	BuildMesh();
}

LODQuadTree::~LODQuadTree() {
}

bool LODQuadTree::Subdivde() {
	if (topLeft != nullptr) {
		//The children were allready subdivied previously and nothing needs to be done...
		return false;
	}

	//Make the quad trees
	topLeft = std::make_unique<LODQuadTree>(Rectangle(bounds.x, bounds.y, bounds.width * 0.5f, bounds.height * 0.5f), LODHeight * heightScalar, this);
	topRight = std::make_unique<LODQuadTree>(Rectangle(bounds.x + (bounds.width * 0.5f), bounds.y, bounds.width * 0.5f, bounds.height * 0.5f), LODHeight * heightScalar, this);
	bottomLeft = std::make_unique<LODQuadTree>(Rectangle(bounds.x + (bounds.width * 0.5f), bounds.y + (bounds.height * 0.5f), bounds.width * 0.5f, bounds.height * 0.5f), LODHeight * heightScalar, this);
	bottomRight = std::make_unique<LODQuadTree>(Rectangle(bounds.x, bounds.y + (bounds.height * 0.5f), bounds.width * 0.5f, bounds.height * 0.5f), LODHeight * heightScalar, this);

	//TODO check to see if this does what i think it does...
	mesh.release();
	mesh = nullptr;
	return true;
}

bool LODQuadTree::LODPosition(Vector3& position) {
	Vector2 center = bounds.Center();
	float dist = position.Distance(Vector3(center.x, 0, center.y));
	if (dist < LODHeight) {
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


bool LODQuadTree::Combine() {
	if (topLeft == nullptr) {
		if (parent == nullptr) {
			//This is the root tree of the structure... min LOD
			return false;
		}
		return parent->Combine();
	} 

	topLeft.release();
	topRight.release();
	bottomLeft.release();
	bottomRight.release();

	BuildMesh();
}

void LODQuadTree::Draw(std::unique_ptr<IRenderer>& renderer, std::unique_ptr<GLSLProgram>& shader) {
	if (!active && mesh != nullptr){
		mesh->Setup();
		active = true;
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


float LODQuadTree::GetHeight(float x, float z) {
	return heightmap[((int)z * rootWidth) + (int)x];
};
