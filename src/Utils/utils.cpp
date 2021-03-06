#include "utils.hpp"

#include <Core/types.h>
#include <Core/logging.h>

#include <fstream>

std::string ReadEntireFile (const std::string& filename) {
	std::ifstream file(filename, std::ios::binary);
	if (!file.is_open()) {
		LOG_ERROR("Could not open " << filename << " when attempting to read entire file");
		return std::string("ERROR_FILE_NOT_READ_CORRECTLY");
	}

	file.seekg(0, std::ios::end);
	auto size = file.tellg();
	file.seekg(0, std::ios::beg);

	std::string buffer;
	buffer.resize((size_t)size);
	file.read(&buffer[0], size);
	return buffer;
}