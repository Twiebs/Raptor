#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include <Core/types.h>

#ifndef PROFILER_PERSISTANT_SAMPLE_COUNT
#define PROFILER_PERSISTANT_SAMPLE_COUNT 64
#endif	//PROFILER_PERSISTANT_SAMPLE_COUNT
#ifndef PROFILER_INSTANTANEOUS_ENTRY_COUNT
#define PROFILER_INSTANTANEOUS_ENTRY_COUNT 128
#endif //PROFILER_INSTANTANEOUS_ENTRY_COUNT

#ifdef PROFILER_ENABLED
#define ProfilerBeginPersistantEntry(name) __ProfilerBeginPersistantEntry(name)
#define ProfilerEndPersistantEntry(name) __ProfilerEndPersistantEntry(name)
#define ProfilerBeginEntry(name) __ProfilerBeginEntry(name)
#define ProfilerEndEntry(name) __ProfilerEndEntry(name)
#define ProfilerReset() __ProfilerReset();
#else
#define ProfilerBeginPersistantEntry(name)
#define ProfilerEndPersistantEntry(name)
#define ProfilerBeginEntry(name)
#define ProfilerEndEntry(name)
#define ProfilerReset()
#endif

struct ProfilerPersistantEntry {
	std::string name;
	U32 entryIndex = 0;
	U32 writeIndex = 0;
	U64 tempTime;
	float elapsedTimes[PROFILER_PERSISTANT_SAMPLE_COUNT];
	U64 elapsedCycles[PROFILER_PERSISTANT_SAMPLE_COUNT];
};

struct ProfilerEntry {
	std::string name;
	double elapsedTime;
	U64 elapsedCycles;
	U64 tempTime;
};

struct Profiler {
	std::vector<U32> persistantEntryIndexStack;
	std::vector<ProfilerPersistantEntry> persistantEntries;
	std::vector<U32> activePersistantEntries;
	std::unordered_map<std::string, U32> persistantEntryNameMap;
	
	U32 newEntryIndex;
	ProfilerEntry entires[PROFILER_INSTANTANEOUS_ENTRY_COUNT];
	std::vector<U32> entryIndexStack;

	void BeginEntry(const char* name);
	void EndEntry(const char* name);
};

Profiler* GetGlobalProfiler();

void __ProfilerBeginPersistantEntry (const char* name);
void __ProfilerEndPersistantEntry (const char* name);
void __ProfilerBeginEntry (const char* name);
void __ProfilerEndEntry (const char* name);
void __ProfilerReset();

