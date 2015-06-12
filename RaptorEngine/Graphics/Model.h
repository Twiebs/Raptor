#pragma once

#include<vector>
#include<Core\ResourceManager.h>
#include<Graphics\Mesh.h>
#include<Graphics\GL\GLSLProgram.h>
#include<Graphics\Resource.h>

#include<memory>

class Mesh;

class Model : public Resource{
friend class ResourceManager;
public:
	Model();
	~Model();

	std::vector<std::unique_ptr<Mesh>> meshes;
private:

};

