#ifndef __RAPTOR_LOGGING_H
#define __RAPTOR_LOGGING_H

#include <sstream>
#include <cassert>
#include <iostream>
#include <vector>

#include <Core/types.h>

enum LogLevel {
	LOGLEVEL_DISABLED,
	LOGLEVEL_ERROR,
	LOGLEVEL_WARNING,
	LOGLEVEL_INFO,
	LOGLEVEL_VERBOSE
};

static inline const char* ToString(LogLevel level) {
	switch (level) {
	case LOGLEVEL_ERROR:	return "ERROR";
	case LOGLEVEL_WARNING: return "WARNING";
	case LOGLEVEL_INFO:	return "INFO";
	case LOGLEVEL_VERBOSE: return "VERBOSE";
	}
	return "UNKNOWN LOG LEVEL!";
}

struct LogEntry {
	LogLevel level;
	std::string message;

	LogEntry(LogLevel level, const std::string& msg)
		: level(level), message(msg) { }
};

// This is templated so that the compiler can constant fold
// The if statements inside of the logging functions based
// on the macro specified log level
template <LogLevel logLevel, bool outputToSTDIO>
struct Log {
	std::stringstream stream;
	std::vector<LogEntry> entries;
	std::vector<U32> addedEntryEventStack;
	const char* loggerName;

	inline bool IsErrorEnabled()	{ return logLevel >= LOGLEVEL_ERROR; } 
	inline bool IsWarningEnabled()	{ return logLevel >= LOGLEVEL_WARNING; }
	inline bool IsInfoEnabled()		{ return logLevel >= LOGLEVEL_INFO; }
	inline bool IsVerboseEnabled()	{ return logLevel >= LOGLEVEL_VERBOSE; }
	inline void AddEntry (LogLevel level) {
		auto message = stream.str();
		entries.emplace_back(level, message);
		stream.clear();
		if (outputToSTDIO) {
			std::cout << "[" << ToString(level) << "::" << loggerName << "] " << message << "\n";
		}
	}

	Log(const char* loggerName) : loggerName(loggerName) { }
};


#define LOG_LEVEL_ERROR 0
#define LOG_LEVEL_WARNING 1
#define LOG_LEVEL_INFO 2
#define LOG_LEVEL_VERBOSE 3

#define GLOBAL_LOG_LEVEL LOG_LEVEL_INFO


#if GLOBAL_LOG_LEVEL >= LOG_LEVEL_ERROR
#define LOG_ERROR(msg) std::cerr << "[ERROR] " << msg << "\n"
#else
#define LOG_ERROR(msg)
#endif

#if GLOBAL_LOG_LEVEL >= LOG_LEVEL_WARNING
#define LOG_WARNING(msg) std::cout << "[WARNING] " << msg << "\n"
#else
#define LOG_WARNING(msg)
#endif

#if GLOBAL_LOG_LEVEL >= LOG_LEVEL_INFO
#define LOG_INFO(msg) std::cout << "[INFO] " << msg << "\n"
#else
#define LOG_INFO(msg)
#endif

#if GLOBAL_LOG_LEVEL >= LOG_LEVEL_VERBOSE
#define LOG_VERBOSE(msg) std::cout << "[VERBOSE] " << msg << "\n";
#else
#define LOG_VERBOSE(msg)
#endif

#define CREATE_LOG_CATEGORY(categoryName, loglevel) global_variable Log<loglevel, true> GENERATEDLOG_##categoryName (#categoryName);

#define LOGERROR(categoryName, msg) if (GENERATEDLOG_##categoryName.IsErrorEnabled()) { GENERATEDLOG_##categoryName.stream << msg; GENERATEDLOG_##categoryName.AddEntry(LOGLEVEL_ERROR); }


// #define LOG_ERROR(categoryName, msg) std::cerr << "[ERROR]" << msg << "\n"



#endif // __RAPTOR_LOGGING_H