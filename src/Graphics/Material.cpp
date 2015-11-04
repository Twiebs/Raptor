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

const char* GetMaterialParameterString (MaterialParameter param) {
	switch (param) {
	case MATERIAL_DIFFUSE: return "Diffuse";
	case MATERIAL_SPECULAR: return "Specular";
	case MATERIAL_NORMAL: return "Normal";
	case MATERIAL_SPECULAR_EXPONENT: return "Specular Exponent";
	}
	return "UNKNOWN MATERIAL PARAMETER!";
}

template <class TArchive>
void serialize(TArchive& archive, MaterialAssetInfo& data) {
	archive (
		data.diffuseTextureFilename,
		data.specularTextureFilename,
		data.normalTextureFilename
	);
}

void SaveMaterialAssetInfo (const MaterialAssetInfo& data, const std::string& filename) {
	std::ofstream stream(filename);
	if (!stream.is_open()) LOG_ERROR("Could not open file when trying to save material file");

	cereal::BinaryOutputArchive archive(stream);
	archive(data);
	stream.close();
}

void LoadMaterialAssetInfo (MaterialAssetInfo* info, const std::string& filename) {
	std::ifstream stream(filename);
	if (!stream.is_open()) LOG_ERROR("Could not open file when trying to load material file");

	cereal::BinaryInputArchive archive(stream);
	archive(*info);

	auto lastSlash = filename.find_last_of('/');
	if (lastSlash = std::string::npos) {
		info->name = filename;
		info->directory = "";
	} else {
		info->name = filename.substr(lastSlash, filename.length() - lastSlash);
		info->directory = filename.substr(0, lastSlash);
	}

	stream.close();
}


void ImportMaterialData (MaterialData* data, const MaterialAssetInfo& info) {
	static auto loadDataIfFilenameIsValid = [](const std::string& directory, const std::string& filename) {
		if (filename == "") return TextureData { };
		return LoadTextureData(directory + filename);
	};

	data->diffuseData = loadDataIfFilenameIsValid(info.directory,  info.diffuseTextureFilename);
	data->normalData = loadDataIfFilenameIsValid(info.directory, info.normalTextureFilename);
	data->specularData = loadDataIfFilenameIsValid(info.directory, info.specularTextureFilename);
}

void FreeMaterialData (MaterialData* data) {
	static auto freePixelsIfValid = [](TextureData* data) {
		if (data->pixels != nullptr) free(data->pixels);
	};

	freePixelsIfValid(&data->diffuseData);
	freePixelsIfValid(&data->normalData);
	freePixelsIfValid(&data->specularData);
}

GLuint CreateTextureFromData(TextureData* data) {
	GLuint result = CreateTextureFromPixels(data->width, data->height, data->pixels, GL_REPEAT);
	return result;
}

void InitMaterial (Material* material, MaterialData* data) {
	static auto createTextureIfValid = [](TextureData* textureData) -> GLuint{
		if (textureData->pixels != nullptr)
			return CreateTextureFromData(textureData);
		return 0;
	};

	material->diffuseMapID = createTextureIfValid(&data->diffuseData);
	material->specularMapID = createTextureIfValid(&data->specularData);
	material->normalMapID= createTextureIfValid(&data->normalData);
}