#pragma once

#include <vector>
#include <Graphics/Mesh.hpp>
#include <Graphics/GL/GLSLProgram.hpp>
#include <memory>

class Mesh;

class Model{
public:
	Model();
	~Model();

	std::vector<std::unique_ptr<Mesh>> meshes;
private:

};
