#pragma once

#include<memory>
#include<vector>

#include<Math\Rectangle.h>
#include<Math\Vector2.hpp>

class QuadTree {
public:
	QuadTree(Rectangle bounds);
	~QuadTree();
	Rectangle bounds;

	std::unique_ptr<QuadTree> topLeft;	//quad1... quad2... ?
	std::unique_ptr<QuadTree> topRight;
	std::unique_ptr<QuadTree> bottomLeft;
	std::unique_ptr<QuadTree> bottomRight;

	bool Insert(Vector2 point);
	void Subdivde();
private:
	int nodeCapacity;
	std::vector<Vector2> nodes;
};