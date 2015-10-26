#ifndef __RAPTOR_LOGGING_H
#define __RAPTOR_LOGGING_H

#include <cassert>
#include <iostream>

#include <Core/Console.hpp>

#define LOG_ERROR(msg) std::cerr << "[ERROR] " << msg << __FILE__ << "::" << __LINE__ << "\n"; Console::Instance().Stream() << msg; Console::Instance().AddEntry(LogLevel::ERROR)
#define LOG_WARNING(msg) std::cout << "[WARNING]" << msg << "\n"
#define LOG_INFO(x) std::cout << "[INFO] " << x << "\n"
#define LOG_DEBUG(x) std::cout << "[DEBUG] " << x << "\n"
#define LOG_VERBOSE(msg) std::cout << "[VERBOSE]" << msg << "\n";

#endif // __RAPTOR_LOGGING_H