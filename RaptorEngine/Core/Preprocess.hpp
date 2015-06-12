#include <Core\Config.hpp>

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