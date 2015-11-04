#pragma once

#include <vector>
#include <string>
#include <sstream>

#include <Core/types.h>
#include <Core/logging.h>





struct Console {
public:
	Console();
	~Console();
	static Console& Instance();
	std::stringstream& Stream();
	void AddEntry(LogLevel level);
private:
	std::stringstream stream;
	std::vector<LogLevel> entryType;
	std::vector<std::string> entries;
};