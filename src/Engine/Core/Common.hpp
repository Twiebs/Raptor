#pragma once
#include <assert.h>
#include <iostream>
#include <stdint.h>


#define ASSET_DIR "../Assets/"
//#define ASSET_DIR "/home/torin/workspace/Raptor/Assets/"
#define ASSET(x) ASSET_DIR x


#define BENCHMARK 1
#define CLOCK_RESOLUTION std::milli
//#define MULTI_THREADED
#define MINIMUM_THREADS 2

//The types that are commonly used throughout the engine
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;

//TODO: Decide if its a good idea to use float32 everywhere...
typedef float float32;
typedef double float64;

//Unsigned int types
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef uint8_t  U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;

typedef int8_t 	S8;
typedef int16_t S16;
typedef int32_t S32;
typedef int64_t S64;

typedef float	F32;
typedef double	F64;

//EngineID system
//Represents handles that index into arrays containing the requested data
//Adds a level of indirection to remove dangling ptrs and allow the engine
//To restructure data as it needs
//TODO these should no longer be exposed to the user so they should just be uint64s
typedef uint64 AssetID;

//Error logging
#define ASSERT(x) assert(x)
//TODO provide console implementation to send error mesages to! 
#define LOG_ERROR(x) std::cerr << "ERROR: " << x << "\n"
#define LOG_INFO(x) std::cout << "INFO: " << x << "\n"
#define LOG_DEBUG(x) std::cout << "[DEBUG] " << x << "\n"

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
