#pragma once

#include <Graphics/Texture.hpp>
#include <GL/glew.h>

enum MaterialParameter {
	MATERIAL_DIFFUSE,
	MATERIAL_SPECULAR,
	MATERIAL_NORMAL,
	MATERIAL_SPECULAR_EXPONENT,
};

inline const char* GetMaterialParameterString (MaterialParameter param) {
	switch (param) {
	case MATERIAL_DIFFUSE: return "Diffuse";
	case MATERIAL_SPECULAR: return "Specular";
	case MATERIAL_NORMAL: return "Normal";
	case MATERIAL_SPECULAR_EXPONENT: return "Specular Exponent";
	}
	return "UNKNOWN MATERIAL PARAMETER!";
}

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

struct MaterialInfo {
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

void SaveMaterialInfo (const MaterialInfo& data, const std::string& filename);
void LoadMaterialInfo (MaterialInfo* data, const std::string& filename);

void InitMaterial(Material* material, MaterialData* data);
void ImportMaterialData(MaterialData* data, const MaterialInfo& info);
void FreeMaterialData(MaterialData* data);

