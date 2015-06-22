#pragma once

#include <stdint.h>

#define GLFW_APPLICATION
#define SDL

//#define BENCHMARK
#define CLOCK_RESOLUTION std::milli
//#define MULTI_THREADED
#define MINIMUM_THREADS 2

//The types that are commonly used throughout the engine
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

//TODO: Decide if its a good idea to use float32 everywhere...
typedef float float32;
typedef double float64;
typedef unsigned int uint;

//Unsigned int types
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

//EngineID system
//Represents handles that index into arrays containing the requested data
//Adds a level of indirection to remove dangaling ptrs and allow the engine
//To restucture data as it needs
typedef uint64 AssetID;
typedef uint64 TaskID;
typedef uint64 EntityID;		//TODO these should no longer be exposed to the user so they should just be uint64s
typedef uint64 EntityUUID;

//Error logging
#define LOG_ERROR(x) std::cout << "ERROR: " << #x << "\n"

//Bechmarking Macros to determine the time taken for a certin task to compleatedTasks
#ifdef BENCHMARK
#include <chrono>
#define BENCHMARK_START(x) auto benchmark_##x##_start = std::chrono::high_resolution_clock::now()
#define BENCHMARK_END(x) auto benchmark_##x##_end = std::chrono::high_resolution_clock::now();	\
	auto benchmark_##x##_diff = benchmark_##x##_end - benchmark_##x##_start; \
	auto benchmark_##x##_time = std::chrono::duration<double, CLOCK_RESOLUTION>(benchmark_##x##_diff).count();
#define BENCHMARK_LOG(x) std::cout << "BENCHMARK_" << #x << ": " << benchmark_##x##_time << "\n"
#endif

#ifndef BENCHMARK
#define BENCHMARK_START(x)
#define BENCHMARK_END(x)
#define BENCHMARK_LOG(x)
#endif
