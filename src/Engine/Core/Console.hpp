#pragma once

#include <vector>
#include <string>
#include <sstream>

enum class LogLevel {
	ERROR,
	WARNING,
	INFO,
	DEBUG
};

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