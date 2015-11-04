#include "Console.hpp"

Console::Console() {

}

Console::~Console() {

}

Console& Console::Instance() {
	static Console instance;
	return instance;
}

std::stringstream& Console::Stream() {
	return stream;
}

void Console::AddEntry(LogLevel level) {
	entryType.push_back(level);
	entries.push_back(stream.str());
	stream.str("");
}

