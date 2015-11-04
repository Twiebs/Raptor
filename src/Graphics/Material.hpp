#pragma once

#include <Graphics/Texture.hpp>
#include <GL/glew.h>

enum MaterialParameter {
	MATERIAL_DIFFUSE,
	MATERIAL_SPECULAR,
	MATERIAL_NORMAL,
	MATERIAL_SPECULAR_EXPONENT,
};

const char* GetMaterialParameterString (MaterialParameter param);

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

struct MaterialAssetInfo {
	std::string name;
	std::string directory;

	std::string diffuseTextureFilename;
	std::string specularTextureFilename;
	std::string normalTextureFilename;
};

struct MaterialData {
	TextureData diffuseData;
	TextureData normalData;
	TextureData specularData;
};

void SaveMaterialAssetInfo (const MaterialAssetInfo& data, const std::string& filename);
void LoadMaterialAssetInfo (MaterialAssetInfo* data, const std::string& filename);

void InitMaterial(Material* material, MaterialData* data);
void ImportMaterialData(MaterialData* data, const MaterialAssetInfo& info);
void FreeMaterialData(MaterialData* data);