#pragma once

#include <vector>
#include <Graphics/Mesh.h>
#include <Graphics/GL/GLSLProgram.h>
#include <memory>

class Mesh;

class Model{
public:
	Model();
	~Model();

	std::vector<std::unique_ptr<Mesh>> meshes;
private:

};
