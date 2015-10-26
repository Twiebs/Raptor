#pragma once

#include <Graphics/Texture.hpp>
#include <GL/glew.h>

enum MaterialParameter {
	MATERIAL_DIFFUSE,
	MATERIAL_SPECULAR,
	MATERIAL_NORMAL,
	MATERIAL_SPECULAR_EXPONENT,
};

std::string GetMaterialParameterString (MaterialParameter param);

struct UniqueMaterial {
	GLuint diffuseMapID = 0;
	GLuint specularMapID = 0;
	GLuint normalMapID = 0;
	~UniqueMaterial();
};

struct Material {
	GLuint diffuseMapID = 0;
	GLuint specularMapID = 0;
	GLuint normalMapID = 0;
};

struct MaterialData {
	std::string diffuseTextureFilename;
	std::string specularTextureFilename;
	std::string normalTextureFilename;
};

void SaveMaterialData (const MaterialData& data, const std::string& filename);
void LoadMaterialData (MaterialData& data, const std::string& filename);