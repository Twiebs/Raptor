#include "Quadtree.hpp"


QuadTree::QuadTree(Rectangle bounds) {
	this->bounds = bounds;
}

QuadTree::~QuadTree() {
}

bool QuadTree::Insert(Vector2 point) {
	if (!bounds.Contains(point)) {
		return false;
		//This point does not belong in the tree
	}

	if (nodes.size() < nodeCapacity) {
		nodes.push_back(point);
		return true;
	}

	else {
		//The quad tree was at capacity and the point must be inserted in a child
		//This node has no children and we must divide
		if (topLeft == nullptr) {
			Subdivde();
		}

		//Try to insert the point in the chilren nodes
		if (topLeft->Insert(point))	return true;
		if (topRight->Insert(point)) return true;
		if (bottomLeft->Insert(point)) return true;
		if (bottomRight->Insert(point)) return true;

		//The point was unable to be inserted into the tree...
		return false;
	}
}

void QuadTree::Subdivde() {
	//We assume that the children are null and we dont have to check..
	//TODO saftey check that the children are indeed null?

	//Make the quad trees
	topLeft = std::make_unique<QuadTree>(Rectangle(bounds.x, bounds.y, bounds.width * 0.5f, bounds.height * 0.5f));
	topRight = std::make_unique<QuadTree>(Rectangle(bounds.x + (bounds.width * 0.5f), bounds.y, bounds.width * 0.5f, bounds.height * 0.5f));
	bottomLeft = std::make_unique<QuadTree>(Rectangle(bounds.x + (bounds.width * 0.5f), bounds.y + (bounds.height * 0.5f), bounds.width * 0.5f, bounds.height * 0.5f));
	bottomRight = std::make_unique<QuadTree>(Rectangle(bounds.x, bounds.y + (bounds.height * 0.5f), bounds.width * 0.5f, bounds.height * 0.5f));

	for (Vector2& point : nodes) {
		if (topLeft->Insert(point)) continue;
		else if (topRight->Insert(point)) continue;
		else if (bottomLeft->Insert(point)) continue;
		else if (bottomRight->Insert(point)) continue;

		//We should never get here
	}
}
