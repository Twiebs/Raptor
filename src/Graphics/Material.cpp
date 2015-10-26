#include "Material.hpp"

#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>

#include <fstream>

#include <Core/logging.h>

UniqueMaterial::~UniqueMaterial() {
	glDeleteTextures(1, &diffuseMapID);
	glDeleteTextures(1, &specularMapID);
	glDeleteTextures(1, &normalMapID);
}

std::string GetMaterialParameterString (MaterialParameter param) {
	switch (param) {
	case MATERIAL_DIFFUSE: return "Diffuse";
	case MATERIAL_SPECULAR: return "Specular";
	case MATERIAL_NORMAL: return "Normal";
	case MATERIAL_SPECULAR_EXPONENT: return "Specular Exponent";
	}
	return "UNKNOWN MATERIAL PARAMETER!";
}

template <class TArchive>
void serialize(TArchive& archive, MaterialData& data) {
	archive (
		data.diffuseTextureFilename,
		data.specularTextureFilename,
		data.normalTextureFilename
	);
}

void SaveMaterialData (const MaterialData& data, const std::string& filename) {
	std::ofstream stream(filename);
	if (!stream.is_open()) LOG_ERROR("Could not open file when trying to save material file");

	cereal::BinaryOutputArchive archive(stream);
	archive(data);
	stream.close();
}

void LoadMaterialData (MaterialData& data, const std::string& filename) {
	std::ifstream stream(filename);
	if (!stream.is_open()) LOG_ERROR("Could not open file when trying to load material file");

	cereal::BinaryInputArchive archive(stream);
	archive(data);
	stream.close();
}


